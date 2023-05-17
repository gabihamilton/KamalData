#include "TCanvas.h"
#include <iostream>
#include "TLorentzVector.h"
#include "TFile.h"
#include "TH1F.h"
#include "TTree.h"
#include "TString.h"
#include <vector>
#include "Analyze.C"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#ifdef __MAKECINT__
#pragma link C++ class vector<TLorentzVector> +;
#pragma link C++ class vector<vector<short> >+;
#endif

using namespace std;

TString inFileName ="/Users/gbibim/Here/data/tree_ggFG_mG2000_v3_MC2016.root";
TFile* f = new TFile(inFileName,"f");
TTree* tr = (TTree*) f->Get("tree");
    Analyze h(tr);


float JpT;
float mT;
float DPhi;
TLorentzVector V4_MET;



//Write cuts here
bool metCut(){
    if (h.MET>200) return (true);
    else return (false);
}

bool VETO(){
    if (h.NElectrons==0 && h.NMuons==0 && h.BTagsDeepCSV==0) return (true); // for electrons, muons, and b tags
    else return (false);
}

bool trackVeto(){
    return ( h.isoElectronTracks == 0 && h.isoMuonTracks == 0 && h.isoPionTracks == 0 ); //pion tracks take care of the tau veto
}

bool AK4JetPtCut(){
    int j;
    for (int i=0; i<h.Jets->size(); i++) {
        if (h.Jets->at(i).Pt()>30) j++;
    }
    return (j>=2);
}

bool AK4JetPhiCut(){
    int j;
    for(int i=0; i<h.Jets->size(); i++){
        if ((fabs(h.Jets->at(i).DeltaPhi(V4_MET))>0.5)) j++;
    }
    return (j>=2);
}

bool AK8JetPhiCut(){
    int j;
    for(int i=0; i<h.JetsAK8->size(); i++){
        if ((fabs(h.JetsAK8->at(i).DeltaPhi(V4_MET))>0.5)) j++;
    }
    return (j>=1);
}

bool LeadAK8PtEtaMass(){
    if (h.JetsAK8->at(0).Pt()>200 && fabs(h.JetsAK8->at(0).Eta())<2.4 && (h.JetsAK8_softDropMass->at(0)>65 || h.JetsAK8_softDropMass->at(0)<105)) return (true);
    else return (false);
}

bool JetQuality(){
    if (h.JetsAK8_NsubjettinessTau2->at(0)/h.JetsAK8_NsubjettinessTau1->at(0)>0.35) return (false);
    else return (true);
}

//this is only for VBF
//RE-DO this guy event selection paragraph 2
bool AK4Separation(){
    if((h.Jets->at(0).Eta())*(h.Jets->at(1).Eta()>0)) return (false);
    if(fabs((h.Jets->at(0).Eta())-(h.Jets->at(1).Eta())<4)) return (false);
}


void ggf(){
    
    gStyle->SetOptStat(1);
    
    //canvas & histos
    TCanvas* C = new TCanvas();
    const int nbins = 22;
    float edges[nbins + 1] = {400.0, 500.0, 600, 700, 800, 900, 1000, 1100, 1200, 1300, 1400, 1500, 1600, 1700, 1800, 1900, 2000, 2100, 2200, 2350, 2550, 2750, 3000};
    TH1F* HmT = new TH1F("mT", "Transverse Mass", nbins, edges);
    
    //Read events
    int nevents = tr->GetEntries();
    int N=0;
    for (int event=0; event<nevents; event++){
        h.GetEntry(event);
        
        V4_MET.SetPtEtaPhiE(h.MET,0,h.METPhi,0);
        
        if(h.JetsAK8->size() == 0){
          continue;}
        
        //JpT = JetPt();
        
        if (metCut() && VETO() && trackVeto() && AK4JetPtCut() && AK4JetPhiCut() && AK8JetPhiCut() && LeadAK8PtEtaMass() && JetQuality()){
            mT = sqrt(2*h.JetsAK8->at(0).Pt()*h.MET*(1-cos(fabs(h.JetsAK8->at(0).DeltaPhi(V4_MET)))));
            
            HmT->Fill(mT);
        }
        
        
    }
    
    HmT->Draw();
    
    C->SaveAs("mT_histo.pdf");
    C->SetLogy();
    C->SaveAs("mT_histo_log.pdf");
    delete C;
    
    
    
}
