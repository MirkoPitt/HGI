#include <iostream>
#include <cmath>
#include <fstream>
#include <string>
#include <unistd.h>
#include <cstdlib>
#include <libgen.h>
#include <iso646.h>
#include <iomanip>
#include <vector>
#include <ctime>
#include <cv.h>
#include <highgui.h>


//OpenCV Stuff
#include "opencv2/opencv.hpp"
//Root Stuff
#include "TApplication.h"
#include "TROOT.h"
//~ #include "Tstyle.h"
#include "TCanvas.h"
#include "TH2F.h"
#include "TGraph.h"
#include "TGraph2D.h"
#include "TLine.h"
#include "TLegend.h"
#include "TBox.h"
#include "TArrow.h"
#include "TFormula.h"
#include "TMath.h"
#include "TF2.h"
#include "TF1.h"

using namespace cv;
using namespace std;


void mouseEvent(int evt, int x, int y, int flags, void* param) 
{                    
    Mat* rgb = (Mat*) param;
    if (evt == CV_EVENT_LBUTTONDOWN) 
    { 
        printf("Pixel intensity at ( %d, %d ) coordinates ->  B: %d , G : %d , R : %d\n", 
        x, y, 
        (int)(*rgb).at<Vec3b>(y, x)[0], 
        (int)(*rgb).at<Vec3b>(y, x)[1], 
        (int)(*rgb).at<Vec3b>(y, x)[2]); 
    }         
}

inline int Index(int i, int j, int NCol){
	return (i+ (j*NCol));
	}

int main(int argc, char * argv[])
{
    int ChooseCamera;
    cout << "Set the camera you want to use: \n 0 -> Integrated one \n 1 -> USB one " << endl;
    cin >> ChooseCamera;
    VideoCapture cam(ChooseCamera); // with 1 -> choose the webcam if it's attached, otherwise choose the integrated one  
    
    if(!cam.isOpened()){  // Controllo di apertura della webcam
        cout << "Impossibile aprire lo stream della webcam. Il programma verrà terminato." << endl;
        return -1;
	}   
        
    //~ double MyWidth = 640; // Limiti 1280x720 copiati da Ave. Sono quelli propri della videocamera come mostrati da Cheese.
    //~ double MyHeight = 480;
    double MyWidth = 1280; // Limiti 1280x720 copiati da Ave. Sono quelli propri della videocamera come mostrati da Cheese.
    double MyHeight = 720;
    cam.set(CV_CAP_PROP_FRAME_WIDTH, MyWidth);  //Set frame width and height
    cam.set(CV_CAP_PROP_FRAME_HEIGHT, MyHeight);
    //~ system("v4l2-ctl --set-fmt-video=width=1280,height=720,pixelformat=1 -d /dev/video1"); //Set frame width and height through a system call
	cam.set(CV_CAP_PROP_FPS, 30); //Set the webcam frame rate
	
	// NB: per poter modificare i valori della frame Width and Height è necessario compilare le librerie OPENCV con
	// le librerie [ v4l2ucp, v4l-utils and libv4l-dev ] già installate nel computer 
	// (vedi: http://stackoverflow.com/questions/16287488/runtime-opencv-highgui-error-highgui-error-v4l-v4l2-vidioc-s-crop-opencv-c)
	
	
	if (ChooseCamera==1) {
		//~ system("v4l2-ctl --set-ctrl exposure_auto=1 -d /dev/video1"); 			//Alternative method to change v4l2 controls
		//~ system("v4l2-ctl --set-ctrl exposure_absolute=5 -d /dev/video1");
		system("v4l2-ctl -c exposure_auto=1 -d /dev/video1");				//system call to adjust the balance variables, sono necessarie le librerie v4l2
		system("v4l2-ctl -c exposure_absolute=5 -d /dev/video1");			//Le chiamate e le impostazioni le ho copiate da Ave, sperando che siano quelle giust
		// in general v4l2-ctl -c command -> comando per cambiare le immpostazioni, lista delle impostazioni possibili le leggi 
		// da bash dando $v4l2-ctl --list-ctrls 
		// documentazione completa delle v4l2 la trovi @ http://ivtvdriver.org/index.php/V4l2-ctl
		cout << "Sys call for camera #" << ChooseCamera << " done!\n";
		//system("say 'settings done baby'");
	}

	if (ChooseCamera==0) {
		//~ //Put here commands for set the integrated camera commands. Generally useless
		//~ system("v4l2-ctl -c exposure_auto=1 -d /dev/video0");				//system call to adjust the balance variables, sono necessarie le librerie v4l2
		//~ system("v4l2-ctl -c exposure_absolute=5 -d /dev/video0");			//Le chiamate e le impostazioni le ho copiate da Ave, sperando che siano quelle giust
		cout << "Sys call for camera #" << ChooseCamera << " done!\n";
	}


	//Controllo dimensionamento immagine acquisita:
	double RealWidth = cam.get(CV_CAP_PROP_FRAME_WIDTH); 				//This function get the Width (in pixels) of the output.
	double RealHeight = cam.get(CV_CAP_PROP_FRAME_HEIGHT);				//This function get the Height (in pixels) of the output.
	cout << " Setted Width: " << MyWidth << "		|| Real Width: " << RealWidth  << endl;
	cout << " Setted Height: " << MyHeight << "		|| Real Height: " << RealHeight  << endl;
	cout << "\n";
	
	
	//Creazione Root object used for analysis
	TApplication myapp("myapp", &argc,argv);
	
	//ROOT Canvas for the current image
	//~ TCanvas * C = new TCanvas ("C","",710,400);
	//~ TH2D * CurrentHisto = new TH2D ("h2d", " ", (int)MyWidth, -(MyWidth/2)+1 , (MyWidth/2), (int)MyHeight, -(MyHeight/2)+1 , (MyHeight/2) );
	//Canvas for the total image 
	TCanvas * C2 = new TCanvas ("C2","",710,400);
	TH2D * TotalHisto = new TH2D ("TotalHisto", " ", (int)MyWidth, -(MyWidth/2)+1 , (MyWidth/2), (int)MyHeight, -(MyHeight/2)+1 , (MyHeight/2) );
	//~ gStyle->SetPalette(1);
	TotalHisto->Draw("CONT");
	
	//~ double RootTempMatrix [int(MyWidth)][int(MyHeight)]; //declaration of a two dimansional array
	//~ int meanX;
	//~ int meanY;
	//~ double VarX;
	//~ double VarY;
	//~ double IntIntegral; 
	double alfa= 0.79; //IntensityCalibration


    namedWindow("Live_Video",WINDOW_NORMAL); 			//Crea una finestra di cui posso modificare le dimensioni (openCV method)
    resizeWindow("Live_Video", RealWidth, RealHeight); 	//Imposta la dimensione iniziale uguale alla dimensione del frame
    int k=0;
    float DisplayHeight = RealHeight / 2 ;
	float DisplayWidth = RealWidth / 2 ;
    Mat ImageBGR(Size(RealWidth, RealHeight), CV_16UC3);
    
    Vec3b intensity;
	//Definisco due array sui quali metto l'immagine istantanea e l'immagine totale
    double * MatIn = new double [int( MyWidth * MyHeight)];
    double * MatSum = new double [int( MyWidth * MyHeight)];
    
    for (int m=0; m < (MyWidth * MyHeight); m++){
		MatSum[m]=0.0;
		}
	
	for(int i=0; i< int(MyWidth); i++){			//Piazzo dentro i valori
		for(int j=0; j< int(MyHeight); j++){	
			TotalHisto->SetBinContent((MyWidth-i+1),(MyHeight-j+1),0);
		}
	}
        
	int IntensityGreen;
	double temp = 0;
    
    ofstream ofile;
    ofile.open("MatrixSumFondo.txt",ios::out);
    
    
		for(;;)
		{
			k++;
			Mat frame;	//non capisco la logica di rinizializzare il frame ad ogni ciclo...ma nel tutorial officiale opencv è così.. Lascio così che sia mai che vi potrebbero essere problemi con il buffer altrimenti
			cam >> frame; // get a new frame from camera
			imshow("Live_Video", frame);
			
			//MouseCallBack Function to get pixel intensity
			//For very good explaination and examples go at -> http://opencvexamples.blogspot.com/2014/01/detect-mouse-clicks-and-moves-on-image.html#.VWVpIs17h5Q
			setMouseCallback("Live_Video", mouseEvent, &frame);
			
			if( (k%10)==0){		//elabora un frame ogni 50 
				
				cam.retrieve(ImageBGR, 0); //Salva in una matrice la figura della webcam
				//~ cout << "\n--------    Inizio filling    --------" << endl;
				for(int i=0; i< int(MyWidth); i++){			//Piazzo dentro i valori
					for(int j=0; j< int(MyHeight); j++){	
						intensity = ImageBGR.at<Vec3b>(j, i);
						IntensityGreen = pow(intensity.val[1],(1.0/alfa));
						//cout << IntensityGreen << endl;
						//~ MatIn[Index(i,j,MyWidth)] = IntensityGreen;
						//~ CurrentHisto->SetBinContent((MyWidth-i+1),(MyHeight-j+1),IntensityGreen);
						MatSum[Index(i,j,MyWidth)] += IntensityGreen;
						if( (k%200)==0){
							TotalHisto->SetBinContent((MyWidth-i+1),(MyHeight-j+1),MatSum[Index(i,j,MyWidth)]);
						}
					}
				}
				
				//In questo modo stampo un asterisco ogni 20 immagini che si sommano
				if( (k%200)==0){
					temp = k/10;
					cout << "Numero di immagni sovraimposte: " << temp << endl;
				}				
				//~ 
				//~ if( (k%400)==0){
					//~ cout << "\n" << endl;
				//~ }
				//~ cout << "Fine filling:" << endl;
				
				//~ CurrentHisto->Draw("CONT");
				//~ C-> Update();
				//~ TotalHisto->Draw("CONT");
				C2 -> Modified();
				C2-> Update();

			}
			
			double px,py,pz;
			
			if (k==3000){
				for(int i=0; i< int(MyWidth); i++){
					for(int j=0; j< int(MyHeight); j++){
						pz=TotalHisto->GetBinContent((MyWidth-i+1),(MyHeight-j+1));
						px=MyHeight-j+1;
						py=MyWidth-i+1;
						
						ofile << px << "	" << py << "	" << pz << endl;
						
						}
					ofile << endl;
					}
					
					break;
				
				}
			

			if(waitKey(30) >= 0) break;
		} 
		// the camera will be deinitialized automatically in VideoCapture destructor
		ofile.close();
	myapp.Run();
    return 0;
}
