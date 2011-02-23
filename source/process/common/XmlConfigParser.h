/**
 * @file XmlConfigParser.h
 * @brief Defines SF1Config class, which is a XML configuration file parser for SF-1 v5.0
 * @author MyungHyun (Kent)
 * @date 2008-09-05
 */

#ifndef _XML_CONFIG_PARSER_H_
#define _XML_CONFIG_PARSER_H_

#include <configuration-manager/TokenizerConfigUnit.h>
#include <configuration-manager/LAConfigUnit.h>
#include <configuration-manager/RankingConfigUnit.h>
#include <configuration-manager/LAManagerConfig.h>
#include <configuration-manager/DocumentManagerConfig.h>
#include <configuration-manager/IndexManagerConfig.h>
#include <configuration-manager/QuerySupportConfig.h>
#include <configuration-manager/RankingManagerConfig.h>
#include <configuration-manager/MiningConfig.h>
#include <configuration-manager/BrokerAgentConfig.h>
#include <configuration-manager/SMiaConfig.h>
#include <configuration-manager/FirewallConfig.h>
#include <configuration-manager/CollectionParameterConfig.h>
#include <configuration-manager/CobraConfig.h>

#include <util/singleton.h>
#include <util/ticpp/ticpp.h>

#include <boost/unordered_set.hpp>
#include <boost/utility.hpp>

#include <string>
#include <sstream>
#include <map>

namespace sf1r
{
namespace ticpp = izenelib::util::ticpp;

// ------------------------- HELPER FUNCTIONS --------------------------

/// @brief  Checks if a given string can be converted to integer form
bool checkIntFormat( const std::string & str );

/// @brief  Converts the given string to lower-case letters (only for ascii)
void downCase( std::string & str );

/// 
/// @brief   The method finds out if the string is true(y, yes) or false(n, no), or neither
/// @return  -1:false, 0:neither,  1:true
/// 
int parseTruth( const string & str );

/// @brief  Parses a given string based on commas ','
void parseByComma( const std::string & str, std::vector<std::string> & subStrList );


///@ brief  The exception class
class XmlConfigParserException : public std::exception
{
	public:
		XmlConfigParserException( const std::string & details )
            : details_(details)
        { }
		~XmlConfigParserException() throw()
        { }

		/// Override std::exception::what() to return details_
        const char* what() const throw()
        {
            return details_.c_str();
        }

		std::string details_; 
};

class XmlConfigParser
{
protected:
    //---------------------------- HELPER FUNCTIONS -------------------------------
    /// @brief  Gets a single child element. There should be no multiple definitions of the element
    /// @param ele                  The parent element
    /// @param name                 The name of the Child element
    /// @param throwIfNoElement     If set to "true", the method will throw exception if there is 
    ///                             no Child Element
    inline ticpp::Element * getUniqChildElement( 
            const ticpp::Element * ele, const std::string & name, bool throwIfNoElement = true ) const;


    /// @brief      The internal method for getAttribute_* methods. Checks if a value exists and retrieves in
    ///             std::string form if it exists. User can decide if the attribute is essential with the 
    ///             attribute throwIfNoAttribute
    /// @param ele                  The element that holds the attribute
    /// @param name                 The name of the attribute
    /// @param val                  The return container of the attribute
    /// @param torhowIfNoAttribute  Throws exception if attribute does not exist.
    /// @return     Returns true  if the attribute is found and has a value. 
    //                      false if the attribute is not found or has no value.
    bool getAttribute( 
            const ticpp::Element * ele, 
            const std::string & name, 
            std::string & val, 
            bool throwIfNoAttribute = true ) const;


    /// @brief  Gets a integer type attribute. User can decide if the attribute is essential 
    ///         with the attribute throwIfNoAttribute
    /// @param ele                  The element that holds the attribute
    /// @param name                 The name of the attribute
    /// @param val                  The return container of the attribute
    /// @param torhowIfNoAttribute  Throws exception if attribute does not exist.
    /// @return     Returns true  if the attribute is found and has a value. 
    //                      false if the attribute is not found or has no value.
    template <class Type>
    inline bool getAttribute_IntType( 
            const ticpp::Element * ele, 
            const std::string & name, 
            Type & val, 
            bool throwIfNoAttribute = true ) const 
    {
        std::string temp;

        if( !getAttribute( ele, name, temp, throwIfNoAttribute ) )
            return false;

        if( checkIntFormat(temp) )
        {
            stringstream ss;
            ss << temp;
            ss >> val;
        }
        else
        {
            throw_TypeMismatch( ele, name, temp );
        }

        return true;
    }

    /// @brief      Overloaded function for getting "int" attributes
    inline bool getAttribute( 
            const ticpp::Element * ele, 
            const std::string & name, 
            int & val, 
            bool throwIfNoAttribute = true ) const 
    {
        return getAttribute_IntType( ele, name, val, throwIfNoAttribute );
    }

    /// @brief      Overloaded function for getting "unsigned int" attributes
    inline bool getAttribute( 
            const ticpp::Element * ele, 
            const std::string & name, 
            unsigned int & val, 
            bool throwIfNoAttribute = true ) const 
    {
        return getAttribute_IntType( ele, name, val, throwIfNoAttribute );
    }

    /// @brief      Overloaded function for getting "int64_t" attributes
    inline bool getAttribute( 
            const ticpp::Element * ele, 
            const std::string & name, 
            int64_t & val, 
            bool throwIfNoAttribute = true ) const 
    {
        return getAttribute_IntType( ele, name, val, throwIfNoAttribute );
    }

#ifdef __x86_64    
    /// @brief      Overloaded function for getting "size_t" attributes
    inline bool getAttribute( 
            const ticpp::Element * ele, 
            const std::string & name, 
            size_t & val, 
            bool throwIfNoAttribute = true ) const 
    {
        return getAttribute_IntType( ele, name, val, throwIfNoAttribute );
    }
#endif    

    /// @brief  Gets a bool type attribute. User can decide if the attribute is essential 
    ///         with the attribute throwIfNoAttribute. 
    /// @details This version will always throw an exception is the given value is neither of 
    ///          "yes/y/no/n" (case insesitive)
    /// @param ele                  The element that holds the attribute
    /// @param name                 The name of the attribute
    /// @param val                  The return container of the attribute
    /// @param torhowIfNoAttribute  Throws exception if attribute does not exist.
    /// @return     Returns true  if the attribute is found and has a value. 
    //                      false if the attribute is not found or has no value.
    inline bool getAttribute( 
            const ticpp::Element * ele, 
            const std::string & name, 
            bool & val, 
            bool throwIfNoAttribute = true ) const;



    inline bool getPortNumber(
            const ticpp::Element * ele, 
            const std::string & name, 
            std::string& val, 
            bool throwIfNoAttribute = true ) const
    {
        int portNo;
        std::string temp;
        getAttribute(ele, name, temp, throwIfNoAttribute);
        stringstream ss;
        ss << temp;
        ss >> portNo;

        if ( portNo < 0 || portNo > 65535 )
            throw_TypeMismatch(ele, name, temp, "0 to 65535");

        val.swap( temp );
        return true;
    }

    // ----------------------------- THROW METHODS -----------------------------

    // 1. ELEMENTS ---------------
    
    /// @brief  Throws an exception when an element does not exist
    /// @param  name  The name of the element
    inline void throw_MultipleElement( const std::string & name ) const
    {
        std::stringstream msg;
        msg << "Multiple definitions of <" << name << "> element";
        throw XmlConfigParserException( msg.str() );
    }

    /// @brief  Throws an exception when an element does not exist
    /// @param  name  The name of the element
    inline void throw_NoElement( const std::string & name ) const
    {
        std::stringstream msg;
        msg << "Definitions of element <" << name << "> is missing";
        throw XmlConfigParserException( msg.str() );
    }

    // 2. ATTRIBUTES ---------------

    // TODO: suggest type, e.g. "yes|y|no|n", "integer type"
    /// @brief          Throws an exception when an attribute is given the wrong data type
    /// @param ele      The Element which holds the attribute
    /// @param name     The name of the attribute
    /// @param valuStr  The value parsed for the attribute, which was incorrect
    inline void throw_TypeMismatch( 
            const ticpp::Element * ele, 
            const std::string & name, 
            const std::string & valueStr = "" ) const
    {
        stringstream msg;
        msg << "<" << ele->Value() << ">, wrong data type is given for attribute \"" << name << "\"";
        if( !valueStr.empty() )
            msg << " value: " << valueStr;
        throw XmlConfigParserException( msg.str() );
    }

    /// @brief          Throws an exception when an attribute is given the wrong data type
    /// @param ele      The Element which holds the attribute
    /// @param name     The name of the attribute
    /// @param valuStr  The value parsed for the attribute, which was incorrect
    /// @param validValuStr  The value(s) which are valid for the attribute
    inline void throw_TypeMismatch( 
            const ticpp::Element * ele, 
            const std::string & name, 
            const std::string & valueStr,
            const std::string & validValueStr) const
    {
        stringstream msg;
        msg << "<" << ele->Value() << ">, wrong data type is given for attribute \"" << name << "\"";
        if( !valueStr.empty() )
            msg << " value: " << valueStr;
        if( !validValueStr.empty() )
            msg << " suggestion : " << validValueStr;
        throw XmlConfigParserException( msg.str() );
    }

    /// @brief          Throws an exception when an attribute is given the wrong data type
    /// @param ele      The Element which holds the attribute
    /// @param name     The name of the attribute
    /// @param valuLong  The value parsed for the attribute, which was incorrect
    /// @param validValuStr  The value(s) which are valid for the attribute
    inline void throw_TypeMismatch( 
            const ticpp::Element * ele, 
            const std::string & name, 
            const long valueLong,
            const std::string & validValueStr) const
    {
        stringstream msg;
        msg << "<" << ele->Value() << ">, wrong data type is given for attribute \"" << name << "\"";
        msg << " value: " << valueLong;
        if( !validValueStr.empty() )
            msg << " suggestion : " << validValueStr;
        throw XmlConfigParserException( msg.str() );
    }

    /// @brief  Throws an exception when an attribute does not exist
    /// @param ele      The Element which holds the attribute
    /// @param name     The name of the attribute
    inline void throw_NoAttribute( const ticpp::Element * ele, const std::string & name ) const
    {
        stringstream msg;
        msg << "<" << ele->Value() << ">, requires attribute \"" << name << "\"";
        throw XmlConfigParserException( msg.str() );
    }

    izenelib::util::UString::EncodingType parseEncodingType(const std::string& encoding_str);

    /// @brief Return true if given id only consists of alphabets, numbers, dash(-) and underscore(_)
    /// @param id The string to be checked
    /// @return true if given id consists of alaphabets, numbers, dash(-) and underscore(_)
    inline bool validateID(const string & id) const
    {
        const char *chars = id.c_str();
        for(unsigned int i = 0; i < id.size(); i++)
        {
            if(!isalnum(chars[i]) && chars[i] != '-' && chars[i] != '_' && chars[i] != '.')
                return false;
        }

        return true;
    }

};

/// @brief   This class parses a SF-1 v5.0 configuration file, in the form of a xml file
/// 
class SF1Config : boost::noncopyable, XmlConfigParser
{
public:
    //----------------------------  PUBLIC FUNCTIONS  ----------------------------

    SF1Config();
    ~SF1Config(); 

    static SF1Config* get()
    {
      return izenelib::util::Singleton<SF1Config>::get();
    }
    
    /// @brief           Starts parsing the configruation file
    /// @param fileName  The path of the configuration file
    /// @details
    /// The configuration file <System>, <Environment>, and"<Document> are processed
    /// 
    bool parseConfigFile( const std::string & fileName ) throw(XmlConfigParserException );

    std::string getResourceDir() const
    {
      return resource_dir_;
    }
    
    /// @brief                  Gets the configuration related to LAManager
    /// @return                 The settings for LAManager
    /// 
    const LAManagerConfig & getLAManagerConfig()
    {
        //laManagerConfig_.setAnalysisPairList( analysisPairList_ );
        boost::unordered_set<AnalysisInfo>::iterator it;
        for( it = analysisPairList_.begin(); it != analysisPairList_.end(); it++ )
        {
            laManagerConfig_.addAnalysisPair( *it );
        }
        return laManagerConfig_;
    }

    /// @brief                  Gets the configuration related to LAManager
    /// @param laManagerConfig  The settings for LAManager
    void getLAManagerConfig( LAManagerConfig & laManagerConfig )
    {
        laManagerConfig = getLAManagerConfig();
    }


    /// @brief                      Gets the configuration related to BrokerAgent
    /// @return                     The settings for BrokerAgent
    const BrokerAgentConfig & getBrokerAgentConfig()
    {
        return brokerAgentConfig_;
    }

    /// @brief                      Gets the configuration related to BrokerAgent
    /// @param brokerAgentConfig    The settings for BrokerAgent
    void getBrokerAgentConfig( BrokerAgentConfig& brokerAgentConfig )
    {
        brokerAgentConfig = brokerAgentConfig_;
    }


    /// @brief                      Gets the configuration related to DocumentManager
    /// @return                     The settings for DocumentManager
    const DocumentManagerConfig & getDocumentManagerConfig( )
    {
        return docManagerConfig_;
    }

    /// @brief                      Gets the configuration related to DocumentManager
    /// @param docManagerConfig     The settings for DocumentManager
    void getDocumentManagerConfig( DocumentManagerConfig & docManagerConfig )
    {
        docManagerConfig = docManagerConfig_;
    }

    const QuerySupportConfig & getQuerySupportConfig( )
    {
        return query_support_config_;
    }


    void getQuerySupportConfig( QuerySupportConfig & query_support_config )
    {
        query_support_config = query_support_config_;
    }


    /// @brief                      Gets the configuration related to RankingManager
    /// @return                     The settings for RankingManager
    const RankingManagerConfig & getRankingManagerConfig( )
    {
        return rankingManagerConfig_;
    }

    /// @brief                      Gets the configuration related to RankingManager
    /// @param logManagerConfig     The settings for RankingManager
    void getRankingManagerConfig( RankingManagerConfig & rankingManagerConfig )
    {
        rankingManagerConfig = rankingManagerConfig_;
    }
    
     
    /// @brief                      Gets the configuration related to Firewall
    /// @return                     The settings for Firewall
    const FirewallConfig& getFirewallConfig( )
    {
        return firewallConfig_;
    }

    /// @brief                      Gets the configuration related to Firewall
    /// @param logManagerConfig     The settings for Firewall
    void getFirewallConfig( FirewallConfig & firewallConfig)
    {
        firewallConfig = firewallConfig;
    }
     /// @brief                      Gets the configuration related to Cobra
    /// @paramCobraConfig     The settings for Cobra
    void getCobraConfig( CobraConfig & cobraConfig)
    {
        cobraConfig = cobraConfig_;
    }
    
    bool getCollectionMetaByName(
        const std::string& collectionName,
        CollectionMeta& collectionMeta
    ) const
    {
        std::map<std::string, CollectionMeta>::const_iterator it =
            collectionMetaMap_.find(collectionName);

        if(it != collectionMetaMap_.end())
        {
            collectionMeta = it->second;
            return true;
        }

        return false;
    }
    
    const std::map<std::string, CollectionMeta>& getCollectionMetaMap()
    {
        return collectionMetaMap_;
    }
    
    bool IsMiningOpen() const
    {
        return mining_switch_;
    }
    
    std::string getIndexRecommendCronString()
    {
        return cronIndexRecommend_;
    }
    
    std::string getIndexerCronString()
    {
        return cronIndexer_;
    }
    
    void setHomeDirectory(const std::string& homeDir)
    {
        homeDir_ = homeDir;
    }

    std::string getHomeDirectory()
    {
        return homeDir_;
    }
private:
    /// @brief                  Parse <System> settings
    /// @param system           Pointer to the Element
    void parseSystemSettings( const ticpp::Element * system );

    /// @brief                  Parse <SiaProcess> settings
    /// @param system           Pointer to the Element
    void parseSMiaProcess( const ticpp::Element * siaProcess );

    /// @brief                  Parse <MiningParameter> settings
    /// @param system           Pointer to the Element
    void parseMiningParameter( const ticpp::Element * miningParameter );

    /// @brief                  Parse <BrokerAgnet> settings
    /// @param system           Pointer to the Element
    void parseBrokerAgent( const ticpp::Element * brokerAgent );

    /// @brief                  Parse <QuerySupport> settings
    /// @param system           Pointer to the Element
    void parseQuerySupport( const ticpp::Element * querySupport );

    /// @brief                  Parse <FireWall> settings
    /// @param system           Pointer to the Element
    void parseFirewall( const ticpp::Element * tgElement );

    void parseParametersDefault( const ticpp::Element * element );

    /// @brief                  Parse <Tokenizer> settings
    /// @param system           Pointer to the Element
    void parseTokenizer( const ticpp::Element * tokenizer );

    /// @brief                  Parse <LanguageAnalyzer> settings
    /// @param system           Pointer to the Element
    void parseLanguageAnalyzer( const ticpp::Element * languageAnalyzer );

    /// @brief                  Parse <Ranking> settings
    /// @param system           Pointer to the Element
    void parseRanking( const ticpp::Element * ranking );

    /// @brief                  Parse <LanguageIdentifier> settings
    /// @param system           Pointer to the Element
    void parseLanguageIdentifier( const ticpp::Element * langid );


    void parseDeploymentSettings( const ticpp::Element * environment );

private:
    //----------------------------  PRIVATE MEMBER VARIABLES  ----------------------------
    // STATIC VALUES -----------------

    /// @brief  Rank value representing "light" setting
    static const float  RANK_LIGHT  = 0.5f;
    /// @brief  Rank value representing "normal" setting
    static const float  RANK_NORMAL = 1.0f;
    /// @brief  Rank value representing "heavy" setting
    static const float  RANK_HEAVY  = 2.0f;
    /// @brief  Rank value representing "max" setting
    static const float  RANK_MAX    = 4.0f;

    /// @brief  Max length for <Date> field
    static const int DATE_MAXLEN = 1024;

    // CONFIGURATION ITEMS ---------------
    
    /// @brief  Configurations for BrokerAgent
    BrokerAgentConfig brokerAgentConfig_;
    
    std::string resource_dir_;
    
    /// @brief  Configurations for query support
    QuerySupportConfig query_support_config_;
    
    bool mining_switch_;

    CollectionParameterConfig default_parameter_;
    
    /// @brief  Configurations for FireWall
    FirewallConfig firewallConfig_;
    
    /// @brief  Configuraitons for LAManager
    LAManagerConfig laManagerConfig_;

    /// @brief  Configurations for RankingManager
    RankingManagerConfig rankingManagerConfig_;

    /// @brief  Configuraitons for DocumentManager
    DocumentManagerConfig docManagerConfig_;

    CobraConfig cobraConfig_;

    // MAPPING TABLES  ----------------------

    // used to check duplicates 

    /// @brief  Maps tokenizer unit IDs to their instances.
    ///         Used when assigning units in document settings
    std::map<std::string, TokenizerConfigUnit> tokenizerConfigNameMap_;

    /// @brief  Maps LA method unit IDs to their instances.
    ///         Used when assigning units in document settings
    std::map<std::string, LAConfigUnit> laConfigIdNameMap_;

    /// @brief  Maps ranking unit IDs to their instances.
    ///         Used when assigning units in document settings
    std::map<std::string, RankingConfigUnit> rankingConfigNameMap_;


    // LISTS ----------------------------

    /// @brief  Stores all the CollectionMeta objects created while parsing
    std::vector<CollectionMeta> collectionMetaList_;
    
    std::map<std::string, CollectionMeta> collectionMetaMap_;

    /// @brief  Stores all the analyzer-tokenizer pairs that are applied to Properties in
    ///         the configuration. 
    boost::unordered_set<AnalysisInfo> analysisPairList_;
    
    std::string cronIndexRecommend_;
    std::string cronIndexer_;

    /// @bried home of configuration files
    std::string homeDir_;

    friend class CollectionConfig;    
};


class CollectionConfig : boost::noncopyable, XmlConfigParser
{
public:
    CollectionConfig();
    ~CollectionConfig(); 

    static CollectionConfig* get()
    {
      return izenelib::util::Singleton<CollectionConfig>::get();
    }
    
    /// @brief           Starts parsing the configruation file
    /// @param fileName  The path of the configuration file
    /// @details
    /// 
    bool parseConfigFile( const string& collectionName , const std::string & fileName ) throw(XmlConfigParserException );

private:
    /// @brief                  Parse <Collection> settings
    /// @param system           Pointer to the Element
    void parseCollectionSettings( const ticpp::Element * collection, CollectionMeta & collectionMeta );

    /// @brief                  Parse <Path> settings
    /// @param system           Pointer to the Element
    void parseCollectionPath( const ticpp::Element * path, CollectionMeta & collectionMeta );

    // 3.3 <Property> parsing --------
    /// @brief                  Parse <Property> settings
    /// @param system           Pointer to the Element
    void parseProperty(
        const ticpp::Element * property,
        CollectionMeta & collectionMeta,
        std::set<std::string> & propertyNameList
    );

    /// @brief                  Parse <Display> settings
    /// @param system           Pointer to the Element
    /// @param propertyConfig   Property settings
    /// 
    void parseProperty_Display( const ticpp::Element * display, PropertyConfig & propertyConfig );

    /// @brief                  Parse <Indexing> settings
    /// @param system           Pointer to the Element
    /// @param propertyConfig   Property settings
    /// 
    void parseProperty_Indexing( const ticpp::Element * indexing, PropertyConfig & propertyConfig );
    
private:
    //----------------------------  PRIVATE MEMBER VARIABLES  ----------------------------
    // STATIC VALUES -----------------

    /// @brief  Rank value representing "light" setting
    static const float  RANK_LIGHT  = 0.5f;
    /// @brief  Rank value representing "normal" setting
    static const float  RANK_NORMAL = 1.0f;
    /// @brief  Rank value representing "heavy" setting
    static const float  RANK_HEAVY  = 2.0f;
    /// @brief  Rank value representing "max" setting
    static const float  RANK_MAX    = 4.0f;

    /// @brief  Max length for <Date> field
    static const int DATE_MAXLEN = 1024;
};


} // namespace sf1r

#endif //_XML_CONFIG_PARSER_H_
