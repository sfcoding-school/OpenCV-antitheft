#include <stdio.h>
#include <opencv/highgui.h>
#include <opencv/cv.h>
#include <unistd.h>

IplImage* cap;
//color è il colore delle linee e dei punti, step è lo step di avanzamento dei for
void crea_griglia_e_uscita(const CvMat *flow, CvMat *cvmat, int step, CvScalar color) {
    int x, y;
    for (y = 0; y < cvmat->rows; y += step) {
        for (x = 0; x < cvmat->cols; x += step) {
            CvPoint2D32f fxy = CV_MAT_ELEM(*flow, CvPoint2D32f, y, x);//singolo punto

            cvLine(cvmat, cvPoint(x, y), cvPoint(cvRound(x + fxy.x), cvRound(y + fxy.y)), color, 1, 8, 0);//singola linea

            if ((fabs(fxy.x) > 14)&&(fabs(fxy.y) > 14)){ //salva l'immagine solo se ci sono più di 14 linee
                cvSaveImage("output.jpg", cap, 0); //salva l'immagine su disco
				puts("OUTPUT\n");//debug
			}
        }
    }
}

int main() {
	//variabili
	CvMat *img_old = 0, *img_new = 0, *flow_ottico = 0, *analisi = 0;
	
    CvCapture* webcam = cvCreateCameraCapture(0);
    
    if (!webcam)//esco con errore se non trova la webcam
        exit(1);

    //cvNamedWindow("PROGETTO 02", 1); //finestra
    puts("**ATTESA**");
	sleep(4);
    puts("**FINE ATTESA**");
    while (1) {
		cap = cvQueryFrame(webcam);//prende un frame dalla webcam
        
		if(!cap)//controllo su cap, break se non c'è
			break;
		cvShowImage("cap", cap);
		
		if(!img_new){//creazione matrici di lavoro (solo la prima volta)
			img_new = cvCreateMat(cap->height, cap->width, CV_8UC1);
			img_old = cvCreateMat(img_new->rows, img_new->cols, img_new->type);
			flow_ottico = cvCreateMat(img_old->rows, img_old->cols, CV_32FC2); //per salvare il risultato di farneback
			analisi = cvCreateMat(img_new->rows, img_new->cols, CV_8UC3); 
		}
		cvCvtColor(cap, img_new, CV_BGR2GRAY); //input cap, output img_new, converte in B/N
		cvCalcOpticalFlowFarneback(img_old, img_new, flow_ottico, 0.5, 3, 15, 3, 5, 1.2, 0);//riempe matrice flow_ottico
		cvCvtColor(img_old, analisi, CV_GRAY2BGR); //input img_old, output analisi
		crea_griglia_e_uscita(flow_ottico, analisi, 16, CV_RGB(255, 0, 0));
		 
		cvShowImage("analisi", analisi);
		
		//SCAMBIO DEI DUE FRAME
		CvMat *temp;//matrice temporanea di appoggio per la macro scambia sotto
		CV_SWAP(img_old, img_new, temp);//scambia img_old e img_new usando temp come variabile d'appoggio

		if ((char) cvWaitKey(10) == 27)//interrompe il while alla pressione di ESC
			break;
	}

    cvReleaseCapture(&webcam);
    exit(0);
}
