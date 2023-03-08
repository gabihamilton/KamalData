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
    if (h.NElectrons==0 && h.NMuons==0) return (true); //add tau photons and b jets
    else return (false);
}

bool trackVeto(){
    return ( h.isoElectronTracks == 0 && h.isoMuonTracks == 0 && h.isoPionTracks == 0 );
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

bool AK4Separation(){
    if((h.Jets->at(0).Eta())*(h.Jets->at(1).Eta()>0)) return (false);
    if(fabs((h.Jets->at(0).Eta())-(h.Jets->at(1).Eta())<4)) return (false);
}


void PlotmT(){
    
    gStyle->SetOptStat(1);
    
    //canvas & histos
    TCanvas* C = new TCanvas();
    TH1F* HmT = new TH1F("mT", "Transverse Mass", 50, 0, 4000);
    
    //Read events
    int nevents = tr->GetEntries();
    int N=0;
    for (int event=0; event<nevents; event++){
        h.GetEntry(event);
        
        V4_MET.SetPtEtaPhiE(h.MET,0,h.METPhi,0);
        
        if(h.JetsAK8->size() == 0){
          continue;}
        
        //JpT = JetPt();
        
        if (metCut() && VETO() && trackVeto() && AK4JetPtCut() && AK4JetPhiCut() && AK8JetPhiCut() && LeadAK8PtEtaMass() && !AK4Separation()){
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
