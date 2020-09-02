#include "allHeader.h"

void Init()
{
    setcam(1,2);   ///Initializing Camera module
    SetSound(); ///Initializing Sound module
}

int main(int argc, char *argv[])
{
    ///Initialising camera and sound module
	Init();

	///Load the cascades///
    if( !face_cascade.load( face_cascade_name ) ){ printf("--(!)Error loading\n"); return -1; };

    ///Set the toggle switch for the Door, Human, Stair
    door_toggledetect = 0;
    door_prevtoggledetect = 0;
    human_toggledetect = 0;
    human_prevtoggledetect = 0;
    stair_toggledetect = 0;
    stair_prevtoggledetect = 0;


	while(1)
	{

Start:  img1 = image_retrieve(1); ///retrieving the image from the camera for processing
        img2 = image_retrieve(2); ///retrieving the image from the camera for processing

        if (!img1 || !img2)
        {
            goto Start;
        }

        cvShowImage("Front Image",img1);
        cvShowImage("Down Image",img2);

        image_data(img1);
        GenerateMatrices(); ///Generating the required number of matrices

        Mat Ground_Image = GndIm.clone();
        Mat Ground_Gray = convertToGray(Ground_Image);
        Mat Ground_Segment = Segmentation(Ground_Gray,3,1);
        Mat Ground_Erode = Erosion(Ground_Segment,1,3,1);
        ground_presence = ground_segregate(Ground_Erode);
        //imshow("Ground",Ground_Erode);

        Mat Front_Image = OrigIm.clone();
        Mat Front_Gray = convertToGray(Front_Image);
        Mat Front_Segment = Segmentation(Front_Gray,5,1);
        Mat Front_Erode = Erosion(Front_Segment,1,3,1);
        //imshow("Front",Front_Erode);

        /** Erosion and Dilation for the images that is needed for Hough Transform **/
        Mat A1 = Erosion(Front_Erode,10,4,1);
        Mat A2 = Dilation(A1,10,4,1);

        Mat B1 = Erosion(Front_Erode,10,4,2);
        Mat B2 = Dilation(B1,10,4,2);

        imshow("Vertical iamge",A2);
        imshow("Horizontal iamge",B2);

        /** Doing the Hough Transform to find the Horizontal and Vertical Lines **/
        Mat VNHgV = DoorHough(A2);
        VNHgV = Dilation(VNHgV,60,4,1);
        imshow("Ver Normal Hough",VNHgV);

        Mat HNHgH = StairHough(B2);
        HNHgH = Dilation(HNHgH,60,4,2);
        imshow("Hor Normal Hough",HNHgH);

        /** Algorithms for Detection of several things **/
        //door_presence = doorsense(VNHgV);
        //stair_presence = stairsense(HNHgH);
        human_presence = humansense(HumanIm);

        decision_algorithm(door_presence,stair_presence,human_presence,ground_presence);

        /** Release the Matrices which were acquired by the user **/
        Ground_Image.release();
        Ground_Gray.release();
        Ground_Segment.release();
        Ground_Erode.release();
        Front_Image.release();
        Front_Gray.release();
        Front_Segment.release();
        Front_Erode.release();A1.release();
        A2.release();
        B1.release();
        B2.release();
        VNHgV.release();
        HNHgH.release();


        cout<<endl<<endl;

        int c = waitKey(10);

        if( (char)c == 'e' || (char)c == 'E' ) { break; }
    }
    return 0;
}
