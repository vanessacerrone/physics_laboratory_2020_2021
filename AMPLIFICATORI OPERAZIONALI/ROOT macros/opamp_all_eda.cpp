#include <fstream>
#include <vector>
#include <cmath>

namespace NSP {
/*---COSTANTI---*/ 

    //nome file 
    string FILE_NAME = "../Data/data_opamp_all.txt";  

    //plot range del fit
    double XMIN = -2;
    double XMAX = 2;
    double YMIN = -18;
    double YMAX = 18;

    //plot range dei residui
    double RESXMIN = XMIN;
    double RESXMAX = XMAX;
    double RESYMIN = -2;
    double RESYMAX = 2;

/*---OGGETTINI CARINI---*/ 

    //vector dei dati + errori
    vector<double> x, y, errX, errY;

    //i due canvas
    //TCanvas *c1, *c2;
    TCanvas *c1;

    //il grafico del fit
    TGraph *plot;
    TGraphErrors *plot_err;

    //il grafico dei residui
    TGraph *residuals;
    TGraphErrors *residuals_err;
    

/*---FUNZIONI---*/ 

    //la funzione matematica del fit
    double myfit(double*, double*);
    //numero di parametri del fit
    double NPAR = 2;

    //funzione per il calcolo dei residui
    TGraph * res(TGraph*);
    TGraphErrors * res(TGraphErrors*);

    //funzione per il fit
    TFitResultPtr fit_fun(TGraphErrors*);
    TFitResultPtr fit_fun(TGraph*);
    
    //funzione per la personalizzazione del grafico fit
    void settings_fit(TGraphErrors*);
    void settings_fit(TGraph*);

    //funzione per la personalizzazione del grafico residui
    void settings_res(TGraphErrors*);
    void settings_res(TGraph*);

    //funzione per l'aggiunta di linee e cose varie nel grafico dei residui
    void linee_res();

    //funzione per la personalizzazione globale dei grafici
    void settings_global();

    //funzione per leggere i dati senza errori da un file
    void read_data(vector<double>&, vector<double>&);

    //funzione per leggere i dati con errori da un file
    void read_data(vector<double>&, vector<double>&, vector<double>&, vector<double>&);

    //funzione per creare il grafico del fit senza errori
    TGraph *plot_fit(vector<double>&, vector<double>&);

    //funzione per creare il grafico del fit con errori
    TGraphErrors *plot_fit(vector<double>&, vector<double>&, vector<double>&, vector<double>&);
}

/*---MAIN---*/

void opamp_all_eda(){

    //leggo i dati con errori dal file
    NSP::read_data(NSP::x, NSP::y, NSP::errX, NSP::errY);

    //creo il grafico del fit con errori
    NSP::plot_err = NSP::plot_fit(NSP::x, NSP::y, NSP::errX, NSP::errY); 
   
    //faccio il fit
    TFitResultPtr fit = NSP::fit_fun(NSP::plot_err);

    //calcolo i residui
	NSP::residuals_err = NSP::res(NSP::plot_err);
    
    //personalizzo il grafico fit
    NSP::settings_fit(NSP::plot_err);

    //personalizzo il grafico residui
    NSP::linee_res();
    NSP::settings_res(NSP::residuals_err); 

    NSP::c1->cd(1);

    TLine *line = new TLine(NSP::XMIN, 0, NSP::XMAX, 0);
    line->SetLineStyle(2);
    line->Draw();

    line = new TLine(0, NSP::YMIN, 0, NSP::YMAX);
    line->SetLineStyle(2);
    line->Draw();


    //personalizzo in modo globale i grafici
    NSP::settings_global();
    
    //NSP::c1->SaveAs("../Plots/opamp_max_plot_projected.png");
    //NSP::c2->SaveAs("../Plots/opamp_max_res_projected.png");
}

/*---FUNZIONI---*/ 

//funzione matematica per il fit
double NSP::myfit(double* x, double* par){   
    Double_t a = par[0];
    Double_t b = par[1];

    Double_t fit_function = 0;

    fit_function = (a * x[0] + b);

    return fit_function;
}

//fit
TFitResultPtr NSP::fit_fun(TGraphErrors* graph) {
    //il fit viene disegnato nel primo canvas
    NSP::c1 = new TCanvas("canvas1", "Fit", 1080, 720);
    NSP::c1->Divide(2, 0);
    NSP::c1->cd(1);

    //creo la funzione di root
    TF1* f1 = new TF1("myfit", myfit, NSP::XMIN, NSP::XMAX, NSP::NPAR);
    f1->SetParNames("a", "b");
    f1->SetLineColor(kRed);
    f1->SetLineStyle(2);
    f1->SetLineWidth(2);

    //faccio il fit
    TFitResultPtr fit_result = graph->Fit("myfit", "SR");
    TFitResultPtr fit_result2 = graph->Fit("myfit2", "SR");
    fit_result->Print("V");
    //disegno il grafico
    graph->Draw("AP");

    return fit_result;
}


//residui
TGraphErrors* NSP::res(TGraphErrors* graph) {
    //i residui vengono disegnati nel secondo canvas
    //NSP::c2 = new TCanvas("canvas2", "Residui", 1080, 720);
    NSP::c1->cd(2);

    //creo vector per i residui
    vector<double> res;

    //calcolo i residui
    for (int i = 0; i < NSP::x.size(); i++) {
		res.push_back(NSP::y[i] - graph->GetFunction("myfit")->Eval(NSP::x[i]));
    }

    //creo il grafico dei residui
    TGraphErrors* res_plot = new TGraphErrors(NSP::x.size(), &NSP::x[0], &res[0], &NSP::errX[0], &NSP::errY[0]);

    //plot grafico residui
    res_plot->Draw("AP"); 

    return res_plot;
}



//personalizzazione grafico fit
void NSP::settings_fit(TGraphErrors* graph) {
    //entro nel primo canvas
    NSP::c1->cd(1);

    //titolo e assi
    graph-> SetTitle("OpAmp Grafico Esplorativo; V_{in} (V); V_{out} (V)");

    //stile e colore
    graph-> SetLineColor(kBlack);
    graph-> SetMarkerStyle(20);
    graph-> SetMarkerColor(kBlack);
    graph-> SetMarkerSize(1);

    gPad->Modified();
    
    //plot range
    graph->GetXaxis()->SetLimits(NSP::XMIN, NSP::XMAX);
    graph->SetMinimum(NSP::YMIN);
    graph->SetMaximum(NSP::YMAX);

    //tick più guardabili
    graph->GetXaxis()->SetTickLength(0.02);
    graph->GetYaxis()->SetTickLength(0.02);
}



//personalizzazione grafico residui 
void NSP::settings_res(TGraphErrors* graph) {
    //entro nel primo canvas
    NSP::c1->cd(2);

    //titolo e assi 
    graph-> SetTitle("Residui; V_{in} (V); V_{out} - fit (V)");

    //colori e cose 
    graph-> SetLineColor(kBlack);
    graph-> SetMarkerStyle(20);
    graph-> SetMarkerColor(kBlack);
    graph-> SetMarkerSize(1);

    gPad->Modified();
    
    //plot range
    graph->GetXaxis()->SetLimits(NSP::RESXMIN, NSP::RESXMAX);
    graph->SetMinimum(NSP::RESYMIN);
    graph->SetMaximum(NSP::RESYMAX);

    //tick più guardabili
    graph->GetXaxis()->SetTickLength(0.02);
    graph->GetYaxis()->SetTickLength(0.02);
}

//linea di zero nel grafico dei residui
void NSP::linee_res() {    
    NSP::c1->cd(2); 

    //creo la linea
    TLine *line = new TLine (NSP::RESXMIN, 0, NSP::RESXMAX, 0); //linea orizzontale sullo zero 

    //personalizzazione
    line->SetLineStyle(2);
    line->SetLineColor(kBlack);

    line->Draw();
}

//personalizzazione globale dei grafici
void NSP::settings_global() {
    //imposto massimo tre cifre prima di usare la notazione scientifica
    TGaxis::SetMaxDigits(3);
    //le tick labels hanno lo stesso numero di cifre significative
    gStyle->SetStripDecimals(kFALSE);
    gStyle->SetImageScaling(3.);
}

//funzione per leggere i dati con errori da un file
void NSP::read_data(vector<double>& x, vector<double>& y, vector<double>& errX, vector<double>& errY) {
    //leggo il file e carico i dati nei vector
    /*---NB: sarebbe meglio usare dei vector ausiliari così si possono fare operazioni e/o propagazioni di errori prima di inserirli nei vector ufficiali---*/
    ifstream f;
    f.open(NSP::FILE_NAME);
    double i = 0;
    while(f >> i) {

        NSP::x.push_back(i);    //prima colonna
        f >> i;
        NSP::y.push_back(i);    //seconda colonna
        f >> i;
        NSP::errX.push_back(i);     //push_back(0) se non ho errori sull'asse x
        f >> i;
        NSP::errY.push_back(i);

    }
    f.close();
}

//creo il grafico con errori
TGraphErrors *NSP::plot_fit(vector<double>& x, vector<double>& y, vector<double>& errX, vector<double>& errY) {
    
    TGraphErrors* graph = new TGraphErrors(NSP::x.size(), &NSP::x[0], &NSP::y[0], &NSP::errX[0], &NSP::errY[0]);

    return graph;
}