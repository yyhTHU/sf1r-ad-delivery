#include "ProductScoreManager.h"
#include "ProductScoreReader.h"
#include "OfflineProductScorerFactory.h"
#include "ProductScoreTable.h"
#include "ProductScoreReader.h"
#include "../util/FSUtil.hpp"
#include "../MiningException.hpp"
#include <configuration-manager/ProductRankingConfig.h>
#include <configuration-manager/MiningConfig.h>
#include <document-manager/DocumentManager.h>
#include <common/SearchCache.h>
#include <common/TableSwapper.h>
#include <node-manager/RequestLog.h>
#include <node-manager/DistributeRequestHooker.h>
#include <node-manager/NodeManagerBase.h>
#include <node-manager/MasterManagerBase.h>
#include <util/scheduler.h>
#include <glog/logging.h>
#include <boost/scoped_ptr.hpp>

using namespace sf1r;

ProductScoreManager::ProductScoreManager(
    const ProductRankingConfig& config,
    const ProductRankingPara& bundleParam,
    OfflineProductScorerFactory& offlineScorerFactory,
    const DocumentManager& documentManager,
    const std::string& collectionName,
    const std::string& dirPath,
    boost::shared_ptr<SearchCache> searchCache)
    : config_(config)
    , offlineScorerFactory_(offlineScorerFactory)
    , documentManager_(documentManager)
    , dirPath_(dirPath)
    , cronJobName_("ProductScoreManager-" + collectionName)
    , searchCache_(searchCache)
{
    createProductScoreTable_(POPULARITY_SCORE);

    addCronJob_(bundleParam);
}

ProductScoreManager::~ProductScoreManager()
{
    izenelib::util::Scheduler::removeJob(cronJobName_);

    for (ScoreTableMap::iterator it = scoreTableMap_.begin();
         it != scoreTableMap_.end(); ++it)
    {
        delete it->second;
    }
}

bool ProductScoreManager::open()
{
    try
    {
        FSUtil::createDir(dirPath_);
    }
    catch (const FileOperationException& e)
    {
        LOG(ERROR) << "exception in FSUtil::createDir: " << e.what();
        return false;
    }

    LOG(INFO) << "Start loading product score directory: " << dirPath_;

    for (ScoreTableMap::iterator it = scoreTableMap_.begin();
         it != scoreTableMap_.end(); ++it)
    {
        ProductScoreTable* table = it->second;
        if (!table->open())
        {
            const std::string& typeName =
                ProductRankingConfig::kScoreTypeName[it->first];
            LOG(ERROR) << "ProductScoreTable::open() failed, score type name: "
                       << typeName;
            return false;
        }
    }

    LOG(INFO) << "End loading product score";
    return true;
}

bool ProductScoreManager::buildCollection()
{
    bool result = true;

    if (isEmpty_())
        return result;

    boost::mutex::scoped_try_lock lock(buildCollectionMutex_);

    if (!lock.owns_lock())
    {
        LOG(INFO) << "exit ProductScoreManager::buildCollection() directly, "
                  << "as it's still in progress for previous call.";
        return result;
    }

    for (ScoreTableMap::iterator it = scoreTableMap_.begin();
         it != scoreTableMap_.end(); ++it)
    {
        if (!buildScoreType_(it->first))
        {
            result = false;
        }
    }

    clearSearchCache_();

    return result;
}

void ProductScoreManager::clearSearchCache_()
{
    if (!searchCache_)
        return;

    LOG(INFO) << "clearing search cache";
    searchCache_->clear();
}

ProductScorer* ProductScoreManager::createProductScorer(
    const ProductScoreConfig& config) const
{
    const ProductScoreTable* table = getProductScoreTable(config.type);

    if (table == NULL)
        return NULL;

    return new ProductScoreReader(config, *table);
}

const ProductScoreTable* ProductScoreManager::getProductScoreTable(
    ProductScoreType type) const
{
    ScoreTableMap::const_iterator it = scoreTableMap_.find(type);

    if (it == scoreTableMap_.end())
        return NULL;

    return it->second;
}

void ProductScoreManager::createProductScoreTable_(ProductScoreType type)
{
    const ProductScoreConfig& config = config_.scores[type];
    const std::string& typeName = ProductRankingConfig::kScoreTypeName[type];

    if (config.weight != 0)
    {
        scoreTableMap_[type] = new ProductScoreTable(dirPath_, typeName);
    }
}

bool ProductScoreManager::isEmpty_() const
{
    return scoreTableMap_.empty();
}

bool ProductScoreManager::buildScoreType_(ProductScoreType type)
{
    const std::string& typeName = ProductRankingConfig::kScoreTypeName[type];
    const ProductScoreConfig& config = config_.scores[type];
    boost::scoped_ptr<ProductScorer> scorer(
        offlineScorerFactory_.createScorer(config));

    if (!scorer)
    {
        LOG(ERROR) << "failed to create offline scorer for " << typeName;
        return false;
    }

    const docid_t startDocId = 1;
    const docid_t endDocId = documentManager_.getMaxDocId();

    LOG(INFO) << "start building product score: " << typeName
              << ", start doc id: " << startDocId
              << ", end doc id: " << endDocId;

    TableSwapper<ProductScoreTable> swapper(*scoreTableMap_[type]);
    swapper.copy(endDocId + 1);

    for (docid_t docId = startDocId; docId <= endDocId; ++docId)
    {
        if (docId % 100000 == 0)
        {
            std::cout << "\rbuilding score for doc id: " << docId
                      << "\t" << std::flush;
        }

        score_t score = scorer->score(docId);
        swapper.writer.setScore(docId, score);
    }
    std::cout << "\rbuilding score for doc id: " << endDocId
              << "\t" << std::endl;

    if (!swapper.writer.flush())
    {
        LOG(ERROR) << "ProductScoreTable::flush() failed, score type: "
                   << typeName;
        return false;
    }

    swapper.swap();
    return true;
}

bool ProductScoreManager::addCronJob_(const ProductRankingPara& bundleParam)
{
    if (!cronExpression_.setExpression(bundleParam.cron))
        return false;

    bool result = izenelib::util::Scheduler::addJob(
        cronJobName_,
        60 * 1000, // check time once in each minute
        0, // start from now
        boost::bind(&ProductScoreManager::runCronJob_, this, _1));

    if (!result)
    {
        LOG(ERROR) << "failed in izenelib::util::Scheduler::addJob(), "
                   << "cron job name: " << cronJobName_;
    }

    return result;
}

void ProductScoreManager::runCronJob_(int calltype)
{
    if ((isEmpty_() || !cronExpression_.matches_now()) && calltype == 0)
        return;
    if (calltype == 0 && NodeManagerBase::get()->isDistributed())
    {
        if (NodeManagerBase::get()->isPrimary())
        {
            MasterManagerBase::get()->pushWriteReq(cronJobName_, "cron");
            LOG(INFO) << "push cron job to queue on primary : " << cronJobName_ ;
        }
        else
        {
            LOG(INFO) << "cron job on replica ignored. ";
        }
        return;
    }
    DISTRIBUTE_WRITE_BEGIN;
    DISTRIBUTE_WRITE_CHECK_VALID_RETURN2;

    CronJobReqLog reqlog;
    if (!DistributeRequestHooker::get()->prepare(Req_CronJob, reqlog))
    {
        LOG(ERROR) << "!!!! prepare log failed while running cron job. : " << __FUNCTION__ << std::endl;
        return;
    }

    buildCollection();

    DISTRIBUTE_WRITE_FINISH(true);
}
