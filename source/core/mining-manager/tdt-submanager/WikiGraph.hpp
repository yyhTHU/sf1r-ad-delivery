#ifndef SF1R_WIKI_GRAPH_HPP_
#define SF1R_WIKI_GRAPH_HPP_
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include "PageRank.hpp"
#include "AdBias.hpp"
#include "LevelDbTable.hpp"
//#include "ConBias.hpp"
//#include "database.h"
#include <algorithm>
#include <icma/icma.h>
#include <icma/openccxx.h>
#include <am/succinct/wat_array/wat_array.hpp>
#include <util/singleton.h>
#include <boost/bimap.hpp>
namespace sf1r
{

class WikiGraph
{
    struct NodeCmp
    {
        bool operator() (Node* node1,Node* node2)
        {
            return node1->GetId() > node2->GetId();
        }

    } NodeCmpOperator;
    std::vector<Node*> nodes_;

    //database db_;
    boost::bimap<std::string, int> title_id;
    std::map<int,std::string> redirect;
    // PageRank pr_;
    //set<int> SubGraph_;
    std::string path_;
    std::string redirpath_;
    std::string stopwordpath_;
    //ConBias contentBias_;
    AdBias* advertiseBias_;
    set<string> stopword_;
    wat_array::WatArray wa_;
    TitleIdDbTable* titleIdDbTable;
    IdTitleDbTable* idTitleDbTable;
public:

    static WikiGraph* getInstance()
    {
        return izenelib::util::Singleton<WikiGraph>::get();
    }

    cma::OpenCC* opencc_;

    WikiGraph();

    ~WikiGraph();

    void Init(const std::string& wiki_path, cma::OpenCC* opencc);

    void SetParam(const std::string& wiki_path, cma::OpenCC* opencc);

    void load(std::istream& is);

    void save(std::ostream& os);

    void initFromDb();

    void init();

    void initStopword();

    void flush();

    void InitSubGaph(const int& index,set<int>& SubGraph,int itertime=1);
    //  void InitGraph();


    //  void link2nodes(const int& i,const int& j);

    void GetTopics(const std::vector<std::pair<std::string,uint32_t> >& relativeWords, std::vector<std::string>& topic_list, size_t limit);

    void SetContentBias(const std::vector<std::pair<std::string,uint32_t> >& relativeWords,PageRank& pr,std::vector<pair<double,std::string> >& ret);

    // void pairBias(pair<string,uint32_t>& ReLativepair,bool reset=false);

    Node* getNode(const std::string&  str,bool& HaveGet);

    Node* getNode(const int&  id,bool& HaveGet);

    int  getIndex(const int&  id);

    int  getIndexByOffset(const int&  offSet);

    void SetAdvertiseAll();

    void SetAdvertiseBias(Node* node);

    int Title2Id(const std::string& title,const int i=0);

    void CalPageRank(PageRank& pr);

    void BuildMap();

    void simplifyTitle();

    void loadAll(std::istream &f );

    void load(std::istream &f, int& id,std::string& name );

    //void InitOutLink();

    std::string ToSimplified(const std::string& name);

    bool AddTitleIdRelation(const std::string& name, const int& id);

    std::string GetTitleById(const int& id);

    int GetIdByTitle(const std::string& title, const int i=0);
};

}

#endif
