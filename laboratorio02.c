#include <stdio.h>
#include <opencv/highgui.h>
#include <opencv/cv.h>

IplImage* cap;

void analizza(const CvMat *flow, CvMat *cvmat, int step, CvScalar color) {
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
	CvMat *img_old = 0, *img_new = 0, *flow_ottico = 0, *riconvertita = 0;
	
    CvCapture* webcam = cvCreateCameraCapture(0);
    
    if (!webcam)//esco con errore se non trova la webcam
        exit(1);

    cvNamedWindow("PROGETTO 02", 1); //finestra
    while (1) {
		cap = cvQueryFrame(webcam);//prende un frame dalla webcam
        
		if(!cap)//controllo su cap, break se non c'è
			break;
		
		if(!img_new){//creazione matrici di lavoro (solo la prima volta)
			img_new = cvCreateMat(cap->height, cap->width, CV_8UC1);
			img_old = cvCreateMat(img_new->rows, img_new->cols, img_new->type);
			flow_ottico = cvCreateMat(img_new->rows, img_new->cols, CV_32FC2); //per salvare il risultato di farneback
			riconvertita = cvCreateMat(img_new->rows, img_new->cols, CV_8UC3); //
		}
		
		cvCvtColor(cap, img_new, CV_BGR2GRAY); //conversione in b/n di cap
		cvCalcOpticalFlowFarneback(img_old, img_new, flow_ottico, 0.5, 3, 15, 3, 5, 1.2, 0);
		cvCvtColor(img_old, riconvertita, CV_GRAY2BGR); //riconversione a colori
		analizza(flow_ottico, riconvertita, 16, CV_RGB(255, 0, 0));//chiamata alla funz analizza
		cvShowImage("PROGETTO 02", cap);//mostra immagine attuale su finestra
			
		CvMat *temp;//matrice temporanea di appoggio per scambia sotto
		CV_SWAP(img_old, img_new, temp);//scambia imgsucc e imgprec usando temp come variabile d'appoggio

		if ((char) cvWaitKey(10) == 27)//interrompe il while alla pressione di ESC
			break;
	}

    cvReleaseCapture(&webcam);
    exit(0);
}
