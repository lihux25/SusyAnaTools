#ifndef ANABASELINE_BASELINEDEF_H
#define ANABASELINE_BASELINEDEF_H

#include "NTupleReader.h"
#include "customize.h"
#include "EventListFilter.h"

#include "Math/VectorUtil.h"

#include <sstream>
#include <iostream>
#include <fstream>

class BaselineVessel
{
private:
    const std::string spec;
    topTagger::type3TopTagger * type3Ptr;
    bool isfastsim;

public:
    std::string jetVecLabel;
    std::string CSVVecLabel;
    std::string METLabel;
    std::string METPhiLabel;
    std::string muonsFlagIDLabel;
    std::string elesFlagIDLabel;

    BaselineVessel(const std::string specialization = "", const std::string filterString = "") : spec(specialization),
      jetVecLabel("jetsLVec"), CSVVecLabel("recoJetsBtag_0"), METLabel("met"), METPhiLabel("metphi") 
    { 
      if(filterString.compare("fastsim") ==0) isfastsim = true; else isfastsim = false; 
    }

    ~BaselineVessel() {delete type3Ptr;};

    void passBaseline(NTupleReader &tr);
    void prepareTopTagger();
    bool GetnTops(NTupleReader *tr) const;
    bool passNoiseEventFilterFunc(NTupleReader &tr);
    bool GetMHT(NTupleReader *tr) const;
    bool passQCDHighMETFilterFunc(NTupleReader &tr);
    bool passFastsimEventFilterFunc(NTupleReader &tr);

    topTagger::type3TopTagger * GetType3Ptr() const {return type3Ptr;};
    inline void operator()(NTupleReader &tr) { passBaseline(tr); }
};

inline void passBaselineFunc(NTupleReader &tr, std::string filterstring)
{
  BaselineVessel blv("", filterstring);
  blv.prepareTopTagger();
  blv.passBaseline(tr);
  blv.GetnTops(&tr);
  blv.GetMHT(&tr);
}

namespace stopFunctions
{
    class CleanJets
    {
    public:        
        void operator()(NTupleReader& tr) {internalCleanJets(tr);}

        void setMuonIso(const std::string muIsoFlag);
        void setElecIso(const std::string elecIsoFlag);
        void setJetCollection(std::string jetVecLabel);
        void setBTagCollection(std::string bTagLabel);
        void setMuonsFlagID(std::string muonsFlagIDLabel);
        void setElesFlagID(std::string elesFlagIDLabel);
        void setEnergyFractionCollections(std::string chargedEMfrac, std::string neutralEMfrac, std::string chargedHadfrac);
        void setForceDr(bool forceDr);
        void setDisable(bool disable);
        void setRemove(bool remove);
        void setElecPtThresh(double minPt);
        void setMuonPtThresh(double minPt);
        void setDisableElec(bool disable);
        void setDisableMuon(bool disable);
        //This option is used to clean up to 1 jet in the minDr cone around the muon if the jet is lower pt than the muon
        //It is designed only for use with the z->inv background to remove muon related radiation from the event
        void setJecScaleRawToFull(std::string jecScaleRawToFullLabel);
        void setPhotoCleanThresh(double photoCleanThresh);

        //NOTE!!! Must add Hadron and EM fraction vectors here

        CleanJets()
        {
            setMuonIso("mini");
            setElecIso("mini");
            setJetCollection("jetsLVec");
            setBTagCollection("recoJetsBtag_0");
            setMuonsFlagID("muonsFlagMedium");
            setElesFlagID("elesFlagVeto");
            setEnergyFractionCollections("recoJetschargedHadronEnergyFraction", "recoJetsneutralEmEnergyFraction", "recoJetschargedEmEnergyFraction");    
            setForceDr(false);
            setRemove(false);
            setDisable(false);
            setElecPtThresh(0.0);
            setMuonPtThresh(0.0);
            setPhotoCleanThresh(-999.9);
            setJecScaleRawToFull("recoJetsJecScaleRawToFull");
        }
        
    private:
        std::string muIsoStr_, elecIsoStr_, jetVecLabel_, bTagLabel_, chargedEMFracLabel_, neutralEMFracLabel_, chargedHadFracLabel_;
        std::string muonsFlagIDLabel_, elesFlagIDLabel_;
        std::string recoJetsJecScaleRawToFullLabel_;
        AnaConsts::IsoAccRec muIsoReq_;
        AnaConsts::ElecIsoAccRec elecIsoReq_;
        double elecPtThresh_;
        double muonPtThresh_;
        double photoCleanThresh_;
        bool remove_;
        bool disableMuon_, disableElec_;
        bool forceDr_;

        int cleanLeptonFromJet(const TLorentzVector& lep, const int& lepMatchedJetIdx, const std::vector<TLorentzVector>& jetsLVec, const std::vector<double>& jecScaleRawToFull, std::vector<bool>& keepJet, const std::vector<double>& neutralEmEnergyFrac, std::vector<TLorentzVector>* cleanJetVec, const double& jldRMax, const double photoCleanThresh = -999.9);
        void internalCleanJets(NTupleReader& tr);
    };

    inline void cleanJets(NTupleReader& tr)
    {
      CleanJets cjh;
      cjh(tr);
    }
}

#endif
