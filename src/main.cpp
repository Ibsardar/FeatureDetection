/*
*   Author: Ibrahim Sardar
*   03/31/2018
*/

#include "pnmio.h"  /* REQUIRES C++11 */
#include "llfd.h"   /* LOW LEVEL FEATURE DETECTION FUNCTIONS HERE */
//#include "llip.h"   /* LOW LEVEL PROCESSING FUNCTIONS HERE */

#include <iostream>
#include <sstream>
#include <exception>
#include <string>
#include "math.h"

// helpers
void say(std::string s) {std::cout<<s<<std::endl;}
void save(Image img, std::string original, pnmio image_manager) {
    // ask for new filename for processed image
    say("Enter a name for the processed version of " + original);
    std::string newfilename;
    std::getline(std::cin, newfilename);
    try {
        image_manager.store_image(newfilename, img);
        say("\""+newfilename+"\" saved successfully.");
    } catch (std::exception &e) {
        say("\""+newfilename+"\" failed to save.");
    }
}
void quicksave(Image img, std::string filename, std::string addon, pnmio image_manager) {
    // insert add-on before ".xxx" and save
    int dot_index = (int) filename.find_last_of(".");
    try {
        image_manager.store_image(filename.insert(dot_index, addon), img);
        say("\""+filename+"\" saved successfully.");
    } catch (std::exception &e) {
        say("\""+filename+"\" failed to save.");
    }
}

int main() {

    // print instructions
    say("Welcome to the Image Feature Detector."
        "This program offers 3 different feature detection algorithms to choose from."
        "Type 'q' or 'quit' to quit.");

    // menu
    while (true) {

        // ask for filename to test
        say("\nEnter a filename (in this exe's directory) of an image to process:");
        std::string filename;
        std::getline(std::cin, filename);

        // check quit flag
        if (filename == "q" || filename == "quit")
            break;

        // read image
        pnmio manager;
        Image input;
        try {input = manager.load_image(filename);}
        catch (std::exception &e) {say(e.what()); break;};

        // notify the user the image is being converted to gray-scale if it is RGB; warn/notify otherwise
        if (input.type == 1) {
            manager.convert_rgb_to_gray(input);
            say("RGB image detected; converted to gray-scale for compatibility.");
        } else if (input.type == 0) {
            say("Gray-scale image detected.");
        } else {
            say("Unknown image-type detected; some features may behave unexpectedly.");
        }

        // ask for processing type
        say("\nEnter a processing type: (type the index)\n"
            "0: ...\n"
            "1: Corner Detection (Harris-Stephens)\n"
            "2: Edge Detection (Canny)\n"
            "3: Line Detection & Fitting (Hough)\n");
        std::string ptype;
        std::getline(std::cin, ptype);
        say("");

        // check quit flag
        if (filename == "quit" || filename == "QUIT")
            break;

        // process image and save it as a new image file
        switch (std::stoi(ptype)) {
        case 0: {
            // save original starting input data vector
            std::vector<unsigned int> original = input.data;
            //...
            break;

        } case 1: {
            say("Enter a window size (recommended: 3-7):");
            std::string window_size;
            std::getline(std::cin, window_size);
            say("Enter a value for sigma (recommended: 1 to 3):");
            std::string sigma;
            std::getline(std::cin, sigma);
            say("Enter a value for alpha (recommended: 0.04 to 0.06):");
            std::string alpha;
            std::getline(std::cin, alpha);
            say("Enter a threshold: (recommended: positive value, increase depending on how noisy your image is)");
            std::string threshold;
            std::getline(std::cin, threshold);
            llfd::detect_corners(input,
                                 std::stod(window_size),
                                 std::stod(sigma),
                                 std::stod(alpha),
                                 std::stod(threshold)); // best parameters for hinges.pnm, hinge.pnm, and building.pnm: 5,1,0.05,50
            // hinge,hinges,building: 5,1,0.05,50
            // keys: 9,2,0.05,300
            // pillsetc: 9,1.5,0.05,300
            //llfd::detect_corners(input,9,1.5,0.05,300);
            save(input, filename, manager);
            break;

        } case 2: {
            say("Enter a window size (recommended: 3-7):");
            std::string window_size;
            std::getline(std::cin, window_size);
            say("Enter a value for sigma (recommended: 1 to 3):");
            std::string sigma;
            std::getline(std::cin, sigma);
            say("Enter a value for alpha (recommended: 0.04 to 0.06):");
            std::string alpha;
            std::getline(std::cin, alpha);
            say("Enter a low threshold: (recommended: 0 to 2.5; this changes depending on image's normal-ness)");
            std::string lowth;
            std::getline(std::cin, lowth);
            say("Enter a high threshold: (recommended: 1 to 3.5; this changes depending on image's normal-ness)");
            std::string highth;
            std::getline(std::cin, highth);
            llfd::detect_edges(input,
                               std::stod(window_size),
                               std::stod(sigma),
                               std::stod(alpha),
                               std::stod(lowth),
                               std::stod(highth)); // works pretty well: 3,1,0.04,0.8,2
            // all: 3,1,0.04,0.8,2
            //llfd::detect_edges(input,3,1,0.04,0.8,2);
            save(input, filename, manager);
            break;

        } case 3: {
            say("Enter a window size (recommended: 3-7):");
            std::string window_size;
            std::getline(std::cin, window_size);
            say("Enter a value for sigma (recommended: 1 to 3):");
            std::string sigma;
            std::getline(std::cin, sigma);
            say("Enter an accumulator size (depends on image size & complexity of image):");
            std::string acc_size;
            std::getline(std::cin, acc_size);
            say("Enter a value for theta delta: (recommended: 5 to 25 degrees)");
            std::string dtheta;
            std::getline(std::cin, dtheta);
            say("Enter a line threshold: (depends on # of non-black pixels in image)");
            std::string lineth;
            std::getline(std::cin, lineth);
            std::vector<std::vector<double> > params =
                llfd::detect_and_fit_lines(input,
                                   std::stod(window_size),
                                   std::stod(sigma),
                                   std::stod(dtheta),
                                   std::stod(acc_size),
                                   std::stod(lineth));
            // hinges,hinges: 3,1,20,512,19 + original canny
            // building,keys: 3,1,25,512,50 * original canny
            // pillsetc: 3,1,25,512,100 + canny: 3,1,0.04,0.5,1.75
            //llfd::detect_edges(input,3,1,0.04,0.65,1.88);
            //std::vector<std::vector<double> > params = llfd::detect_and_fit_lines(input,3,1,25,128,100);
            for (int i=0; i<params.size(); i++) {
                std::cout<<"Line Parameter: "<<params[i][0]<<", "<<params[i][1]<<std::endl;
            }
            save(input, filename, manager);
            break;

        } default:
            say("Input could not be understood.");
            break;
        }//end switch
    }

    // end program
    return 0;
}
