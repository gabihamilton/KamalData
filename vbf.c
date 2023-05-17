#include "TCanvas.h"
#include <iostream>
#include "TLorentzVector.h"
#include "TFile.h"
#include "TH1F.h"
#include "TTree.h"
#include "TString.h"
#include <vector>
#include "AnalyseVBF.C"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#ifdef __MAKECINT__
#pragma link C++ class vector<TLorentzVector> +;
#pragma link C++ class vector<vector<short> >+;
#endif

using namespace std;

TString inFileName ="/Users/gbibim/Here/KamalData/data/tree_VBFG_mG2000_v3_MC2016.root";
TFile* f = new TFile(inFileName,"f");
TTree* tr = (TTree*) f->Get("tree");
    AnalyseVBF h(tr);


float JpT;
float mT;
float DPhi;
TLorentzVector V4_MET;
TLorentzVector Jet1;
TLorentzVector Jet2;




//Write cuts here

//pt_miss>200GeV
bool metCut(){
    if (h.MET>200) return (true);
    else return (false);
}

//Veto: electrons muons and b tags
bool VETO(){
    if (h.NElectrons==0 && h.NMuons==0 && h.BTagsDeepCSV==0) return (true); // for electrons, muons, and b tags
    else return (false);
}

//Veto: tau                          
bool trackVeto(){
    return ( h.isoElectronTracks == 0 && h.isoMuonTracks == 0 && h.isoPionTracks == 0 ); //pion tracks take care of the tau veto
}


//At least 2 AK4 with PT>30GeV
/*bool AK4JetPtCut(){
    int j;
    for (int i=0; i<h.Jets->size(); i++) {
        if (h.Jets->at(i).Pt()>30) j++;
    }
    return (j>=2);
}*/

//At least 2 AK4 with deltaPhi>0.5 and PT>30GeV
bool AK4JetPtPhiCut(int i){
    int j;
    //for(int i=0; i<h.Jets->size(); i++){
    if ((h.Jets->at(i).Pt()>30 && fabs(h.Jets->at(i).DeltaPhi(V4_MET))>0.5)){ return (true); //j++;
    }
    else{
        return (false);
    }
    //return (j>=2);
}

//DeltaR from leading AK8
bool AK4Separation(int i){
    int j;
    //for(int i=0; i<h.Jets->size(); i++){
        float DeltaPhi = fabs(h.JetsAK8->at(0).Phi()-h.Jets->at(i).Phi());
        float DeltaEta = fabs(h.JetsAK8->at(0).Eta()-h.Jets->at(i).Eta());
        float DeltaR = sqrt(DeltaPhi*DeltaPhi + DeltaEta*DeltaEta);
        
    if (DeltaR>0.8) return (true);//j++;
    else return (false);
    //}
    //return (j>=2);
}


bool validAK4(){
    vector<int> indices;
    int j;
    for(int i=0; i<h.JetsAK8->size(); i++){
        if(AK4JetPtPhiCut(i) && AK4Separation(i)){
            indices.push_back(i);
        }
    }
    
    if(indices.size()>=2){
        Jet1.SetPtEtaPhiM(h.Jets->at(indices[0]).Pt(),h.Jets->at(indices[0]).Eta(),h.Jets->at(indices[0]).Phi(),h.Jets->at(indices[0]).M());
        Jet2.SetPtEtaPhiM(h.Jets->at(indices[1]).Pt(),h.Jets->at(indices[1]).Eta(),h.Jets->at(indices[1]).Phi(),h.Jets->at(indices[1]).M());

        if(h.Jets->at(indices[0]).Eta()*h.Jets->at(indices[1]).Eta()<0 && fabs(h.Jets->at(indices[0]).Eta()-h.Jets->at(indices[1]).Eta())<4 && (Jet1+Jet2).M()>500){
            return (true);
        }
        else return (false);
    }
    else{
        return (false);
    }
}


//At least 2 AK8 with deltaPhi>0.5
/*bool AK8JetPhiCut(){
    int j;
    for(int i=0; i<h.JetsAK8->size(); i++){
        if ((fabs(h.JetsAK8->at(i).DeltaPhi(V4_MET))>0.5)) j++;
        
    }
    return (j>=1);
}*/

//Leading AK8 pt, eta and mass
bool LeadAK8PtEtaMass(){
    if (h.JetsAK8->at(0).Pt()>200 && fabs(h.JetsAK8->at(0).Eta())<2.4 && h.JetsAK8_softDropMass->at(0)>65 && h.JetsAK8_softDropMass->at(0)<105) return (true);
    else{
        return (false);
    }
}

//tau21 HP:<0.35, LP:0.35-0.75
bool JetQuality(){
    if(h.JetsAK8_NsubjettinessTau2->at(0)/h.JetsAK8_NsubjettinessTau1->at(0)<0.35){
        //&& h.JetsAK8_NsubjettinessTau2->at(0)/h.JetsAK8_NsubjettinessTau1->at(0)<0.75){
        return (true);
    }
    else{
        return (false);
    }
}



void vbf(){
    
    gStyle->SetOptStat(1);
    
    //canvas & histos
    TCanvas* C = new TCanvas();
    const int nbins = 22;
    float edges[nbins + 1] = {400.0, 500.0, 600, 700, 800, 900, 1000, 1100, 1200, 1300, 1400, 1500, 1600, 1700, 1800, 1900, 2000, 2100, 2200, 2350, 2550, 2750, 3000};
    TH1F* HmT = new TH1F("mT", "Transverse Mass", nbins, edges);
    
    //Read events
    int nevents = tr->GetEntries();
    cout << nevents << endl;
    int N=0;
    for (int event=0; event<nevents; event++){
        h.GetEntry(event);
        
        V4_MET.SetPtEtaPhiE(h.MET,0,h.METPhi,0);
        
        if(h.JetsAK8->size() == 0){
          continue;
        }
        
        //JpT = JetPt();
        
        if (metCut() && VETO() && trackVeto() && LeadAK8PtEtaMass() && validAK4() && JetQuality()){
            mT = sqrt(2*h.JetsAK8->at(0).Pt()*h.MET*(1-cos(fabs(h.JetsAK8->at(0).DeltaPhi(V4_MET)))));
            
            HmT->Fill(mT);
        }
        
        
        
    }
    
    HmT->Draw();
    
    C->SaveAs("output/mT_histo.png");
    C->SetLogy();
    C->SaveAs("output/mT_histo_log.png");
    delete C;
    
    
    
}

