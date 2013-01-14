#ifndef WAVLET_FACTORY_HPP_
#define WAVLET_FACTORY_HPP_

#include "wavelet-tree/wat_array.hpp"
#include "wavelet-tree/wavelet_matrix.hpp"

namespace sf1r
{

enum WAVLETTYPE{WAT_ARRAY,WAVLET_MATRIX};

class WavletTree    
{    
public:    
    WavletTree(){}
    virtual ~WavletTree(){}
    virtual void Init(const std::vector<uint64_t>& array) = 0;
    virtual uint64_t Freq(uint64_t c) const = 0;
    virtual uint64_t Rank(uint64_t c, uint64_t pos) const = 0;
    virtual uint64_t Lookup(uint64_t pos) const = 0;
    virtual uint64_t Select(uint64_t c, uint64_t rank) const = 0;
    virtual void QuantileRangeAll(uint64_t begin_pos,uint64_t end_pos, vector<uint64_t>& ret,const BitTrie& filter) const = 0;
};    

class WavletTree1: public WavletTree 
{    
     MyWatArray wa_;
public:    
     WavletTree1(){}
     ~WavletTree1(){}
     void Init(const std::vector<uint64_t>& array){ wa_.Init(array);}
     uint64_t Freq(uint64_t c) const{return wa_.Freq(c);}
     uint64_t Rank(uint64_t c, uint64_t pos) const{return wa_.Rank(c,pos);}
     uint64_t Lookup(uint64_t pos) const{return wa_.Lookup(pos);}
     uint64_t Select(uint64_t c, uint64_t rank) const{return wa_.Select(c,rank);}
     void QuantileRangeAll(uint64_t begin_pos,uint64_t end_pos, vector<uint64_t>& ret,const BitTrie& filter) const{ wa_.QuantileRangeAll(begin_pos,end_pos,ret,filter);}
};    

class WavletTree2: public WavletTree
{    
     MyWaveletMatrix wa_;
public: 
     WavletTree2(){};
     ~WavletTree2(){};   
     void Init(const std::vector<uint64_t>& array){ wa_.Init(array);};
     uint64_t Freq(uint64_t c) const{return wa_.Freq(c);};
     uint64_t Rank(uint64_t c, uint64_t pos) const{return wa_.Rank(c,pos);};
     uint64_t Lookup(uint64_t pos) const{return wa_.Lookup(pos);};
     uint64_t Select(uint64_t c, uint64_t rank) const{return wa_.Select(c,rank);};
     void QuantileRangeAll(uint64_t begin_pos,uint64_t end_pos, vector<uint64_t>& ret,const BitTrie& filter) const{ wa_.QuantileRangeAll(begin_pos,end_pos,ret,filter);};
};    

class WavletFactory    
{    

public:  
    WavletFactory(){};
    ~WavletFactory(){};   
    WavletTree* CreateWavletTree(enum WAVLETTYPE wavlettype)    
    {    
        if(wavlettype == WAT_ARRAY)   
          return new WavletTree1();    
        else if(wavlettype == WAVLET_MATRIX)    
          return new WavletTree2();   
        else 
          return NULL;    
    }    
};  


}
#endif //_
