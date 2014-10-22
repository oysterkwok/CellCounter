//
//  main.cpp
//  CellCounter
//
//  Created by Haoquan Guo on 1/18/12.
//  Copyright 2012 NYU. All rights reserved.
//
//  rgb: 0 is b; 1 is g; 2 is r;

#include <iostream>
#include "cv.h"
#include "highgui.h"
#include "dirent.h"
#include <vector>
#include <fstream>
#include "stdio.h"

void cpypixel(int _width, int _height, IplImage *_src, IplImage *_dst);
void getrgb(int _width, int _height, IplImage *srcimg, uchar *rgb);
void setrgb(int _width, int _height, IplImage *dstimg, uchar *rgb);
void setwhitecolor(int _width, int _height, IplImage *dstimg);
void setblackcolor(int _width, int _height, IplImage *dstimg);
void setredcolor(int _width, int _height, IplImage *dstimg);
void setbluecolor(int _width, int _height, IplImage *dstimg);
bool isDark(uchar *rgb);
bool isBlack(uchar *rgb);
bool isWhite(uchar *rgb);
bool isRed(uchar *rgb);
bool isBlue(uchar *rgb);

void cpypixel(int _width, int _height, IplImage *srcimg, IplImage *dstimg){
    uchar *rgb = new uchar[3];
    getrgb(_width, _height, srcimg, rgb);
    setrgb(_width, _height, dstimg, rgb);
}

void getrgb(int _width, int _height, IplImage *srcimg, uchar *rgb){
    int nchannels = srcimg->nChannels;
    int step = srcimg->widthStep;
    uchar *sdata = ( uchar* )srcimg->imageData;
    for (int i = 0; i < 3; i ++) {
        rgb[i] = sdata[_height*step + _width*nchannels + i];
    }
}

void setrgb(int _width, int _height, IplImage *dstimg, uchar *rgb){
    int nchannels = dstimg->nChannels;
    int step = dstimg->widthStep;
    uchar *ddata = ( uchar* )dstimg->imageData;
    for (int i = 0; i < 3; i ++) {
        ddata[_height*step + _width*nchannels + i] = rgb[i];
    }
}

void setwhitecolor(int _width, int _height, IplImage *dstimg){
    uchar *rgb = new uchar[3];
    rgb[0] = 255;
    rgb[1] = 255;
    rgb[2] = 255;
    setrgb(_width, _height, dstimg, rgb);
}

void setblackcolor(int _width, int _height, IplImage *dstimg){
    uchar *rgb = new uchar[3];
    rgb[0] = 0;
    rgb[1] = 0;
    rgb[2] = 0;
    setrgb(_width, _height, dstimg, rgb);
}

void setredcolor(int _width, int _height, IplImage *dstimg){
    uchar *rgb = new uchar[3];
    rgb[0] = 0;
    rgb[1] = 0;
    rgb[2] = 255;
    setrgb(_width, _height, dstimg, rgb);
}

void setbluecolor(int _width, int _height, IplImage *dstimg){
    uchar *rgb = new uchar[3];
    rgb[0] = 255;
    rgb[1] = 0;
    rgb[2] = 0;
    setrgb(_width, _height, dstimg, rgb);
}

bool isDark(uchar *rgb){
    if (rgb[0] < 70 && rgb[1] < 70 && rgb[2] < 70) {
        return true;
    }
    //    if (rgb[2] + rgb[1] - rgb[0] - rgb[0] < 50) {
    //        return true;
    //    }
    return false;
}

bool isBlack(uchar *rgb){
    if (rgb[0] < 20 && rgb[1] < 20 && rgb[2] < 20) {
        return true;
    }
    return false;
}

bool isWhite(uchar *rgb){
    if (rgb[0] > 240 && rgb[1] > 240 && rgb[2] > 240) {
        return true;
    }
    return false;
}

bool isRed(uchar *rgb){
    if (rgb[0] < 20 && rgb[1] < 20 && rgb[2] > 240) {
        return true;
    }
    return false;
}

bool isBlue(uchar *rgb){
    if (rgb[0] > 240 && rgb[1] < 20 && rgb[2] < 20) {
        return true;
    }
    return false;
}

int main (int argc, const char * argv[])
{
    
    IplImage *img = 0;
    ofstream fout;
    fout.open("./Result.txt", fstream::app);
    vector<string> filelist;
    DIR *pDIR;
    struct dirent *entry;
    pDIR=opendir("./in");
    if (pDIR) {
        while((entry = readdir(pDIR))){
            if( strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0){
                cout << entry->d_name << "\n";
                filelist.push_back(entry->d_name);
            }else{
                continue;
            }
        }
        closedir(pDIR);
    }
    for (int k = 0; k < filelist.size(); k ++) {
        if (k > 4) {
            return 0;
        }
        string path = "./in/";
        string file = filelist[k];
        path.append(file);
        cout << path << "\n";
        img = cvLoadImage(path.c_str(), CV_LOAD_IMAGE_COLOR);
        
        
        if (img == 0) {
            std::cout << "error.\n";
            return 0;
        }
        else{
            int width = img->width;
            int height = img->height;
            int depth = img->depth;
            int nchannels = img->nChannels;
            
            IplImage *dst = cvCreateImage(cvSize(width, height), depth, nchannels);
            for (int x = 0; x < width; x ++) {
                for (int y = 0; y < height; y ++) {
                    setblackcolor(x, y, dst);
                }
            }
            
            //cvCopy(img, dst, NULL);
            // #1 copy only the full circle to dst
            IplImage *tmp = cvCreateImage(cvSize(width, height), depth, nchannels);
            for (int x = 0; x < width; x ++) {
                for (int y = 0; y < height; y ++) {
                    setblackcolor(x, y, tmp);
                }
            }
            int *bin = new int[2*width*height];
            int head = 0;
            int tail = 1;
            bin[head + 0] = width / 2;
            bin[head + 1] = height / 2;
            setwhitecolor(width / 2, height / 2, tmp);
            bool ok = false;
            while (!ok) {
                ok = true;
                while (head < tail) {
                    int i = head;
                    uchar *rgb = new uchar[3];
                    getrgb(bin[i*2], bin[i*2+1], img, rgb);
                    int x = bin[i*2] - width / 2;
                    int y = bin[i*2+1] - height / 2;
                    //std::cout << rgb[0] << ", " << rgb[1] << ", " << rgb[2] << "\n";
                    if (!isDark(rgb) && x*x + y*y < 120000) {
                        setrgb(bin[i*2], bin[i*2+1], dst, rgb);
                        ok = false;
                        // north
                        if (bin[i*2+1] > 0) {
                            getrgb(bin[i*2], bin[i*2+1] - 1, tmp, rgb);
                            if (!isWhite(rgb)) {
                                bin[tail*2] = bin[i*2];
                                bin[tail*2+1] = bin[i*2+1] - 1;
                                setwhitecolor(bin[tail*2], bin[tail*2+1], tmp);
                                tail ++;
                            }
                        }
                        // south
                        if (bin[i*2+1] < height - 1) {
                            getrgb(bin[i*2], bin[i*2+1] + 1, tmp, rgb);
                            if (!isWhite(rgb)) {
                                bin[tail*2] = bin[i*2];
                                bin[tail*2+1] = bin[i*2+1] + 1;
                                setwhitecolor(bin[tail*2], bin[tail*2+1], tmp);
                                tail ++;
                            }
                        }
                        // west
                        if (bin[i*2] > 0) {
                            getrgb(bin[i*2] - 1, bin[i*2+1], tmp, rgb);
                            if (!isWhite(rgb)) {
                                bin[tail*2] = bin[i*2] - 1;
                                bin[tail*2+1] = bin[i*2+1];
                                setwhitecolor(bin[tail*2], bin[tail*2+1], tmp);
                                tail ++;
                            }
                        }
                        // east
                        if (bin[i*2] < width - 1) {
                            getrgb(bin[i*2] + 1, bin[i*2+1], tmp, rgb);
                            if (!isWhite(rgb)) {
                                bin[tail*2] = bin[i*2] + 1;
                                bin[tail*2+1] = bin[i*2+1];
                                setwhitecolor(bin[tail*2], bin[tail*2+1], tmp);
                                tail ++;
                            }
                        }
                    }
                    head ++;
                }
            }
            cvReleaseImage(&tmp);
            std::cout << tail << "\n";
            cvReleaseImage(&img);
            remove(path.c_str());
            cvSaveImage("/Users/oyster/Desktop/temp1.bmp", dst);
            
            // #2 turn all red and blue as black
            for (int i = 0; i < tail; i ++) {
                uchar *rgb = new uchar[3];
                getrgb(bin[i*2], bin[i*2+1], dst, rgb);
                if (isRed(rgb) || isBlue(rgb)) {
                    setblackcolor(bin[i*2], bin[i*2+1], dst);
                }
            }
            
            // #3 find edges the mark the inner pixel as red, and outter pixel as blue
            for (int i = 0; i < tail; i ++) {
                if (bin[i*2] < 10 || bin[i*2] > width - 10 || bin[i*2+1] < 10 || bin[i*2+1] > height - 10) {
                    continue;
                }
                int counter = 0;
                int avail = 0;
                uchar *thisrgb = new uchar[3];
                getrgb(bin[i*2], bin[i*2+1], dst, thisrgb);
                for (int j = -10; j < 10; j ++) {
                    for (int k = -10; k < 10; k ++) {
                        int x = bin[i*2] + j;
                        int y = bin[i*2+1] + k;
                        uchar *rgb = new uchar[3];
                        getrgb(x, y, dst, rgb);
                        if (isRed(rgb) || isBlue(rgb) || isBlack(rgb)) {
                            continue;
                        }
                        avail ++;
                        if (thisrgb[0] + 9 < rgb[0] && thisrgb[2] + 8 + thisrgb[1] < rgb[2] + rgb[1]) {
                            counter ++;
                        }
                    }
                }
                if (counter > avail * 0.55 && avail > 200) {
                    setbluecolor(bin[i*2], bin[i*2+1], dst);
                }
                else if (counter > avail * 0.45 && avail > 200){
                    setredcolor(bin[i*2], bin[i*2+1], dst);
                }
            }
            cvSaveImage("/Users/oyster/Desktop/temp2.bmp", dst);
            
            
            // #4 fliter noise
            
            IplImage *tmp1 = cvCreateImage(cvSize(width, height), depth, nchannels);
            for (int x = 0; x < width; x ++) {
                for (int y = 0; y < height; y ++) {
                    setblackcolor(x, y, tmp1);
                }
            }
            
            for (int i = 0; i < tail; i ++) {
                uchar *thisrgb = new uchar[3];
                getrgb(bin[i*2], bin[i*2+1], dst, thisrgb);
                if (!isBlue(thisrgb) && !isRed(thisrgb)) {
                    continue;
                }
                int *bin1 = new int[2*width*height];
                int head1 = 0;
                int tail1 = 1;
                bin1[head1*2 + 0] = bin[i*2];
                bin1[head1*2 + 1] = bin[i*2+1];
                
                setredcolor(bin1[head1*2], bin1[head1*2+1], tmp1);
                
                int minx = bin[i*2];
                int miny = bin[i*2+1];
                int maxx = bin[i*2];
                int maxy = bin[i*2+1];
                int litcount = 0;
                bool ok1 = false;
                while (!ok1) {
                    ok1 = true;
                    while (head1 < tail1) {
                        int i1 = head1;
                        uchar *rgb = new uchar[3];
                        getrgb(bin1[i1*2], bin1[i1*2+1], dst, rgb);
                        //std::cout << rgb[0] << ", " << rgb[1] << ", " << rgb[2] << "\n";
                        if (isBlue(rgb) || isRed(rgb)) {
                            litcount ++;
                            if (bin1[i1*2] < minx) {
                                minx = bin1[i1*2];
                            }
                            if (bin1[i1*2] > maxx) {
                                maxx = bin1[i1*2];
                            }
                            if (bin1[i1*2+1] < miny) {
                                miny = bin1[i1*2+1];
                            }
                            if (bin1[i1*2+1] > maxy) {
                                maxy = bin1[i1*2+1];
                            }
                            //setrgb(bin1[i1*2], bin1[i1*2+1], dst, rgb);
                            ok1 = false;
                            
                            for (int j1 = -3; j1 < 3; j1 ++) {
                                for (int k1 = -3; k1 < 3; k1 ++) {
                                    if (j1*j1 + k1*k1 <= 10) {
                                        int x1 = j1 + bin1[i1*2];
                                        int y1 = k1 + bin1[i1*2+1];
                                        if (x1 > 0 && x1 < width && y1 > 0 && y1 < height) {
                                            getrgb(x1, y1, tmp1, rgb);
                                            if (!isRed(rgb)) {
                                                bin1[tail1*2] = x1;
                                                bin1[tail1*2+1] = y1;
                                                setredcolor(bin1[tail1*2], bin1[tail1*2+1], tmp1);
                                                tail1 ++;
                                                
                                            }
                                        }
                                    }
                                }
                            }
                            
                            //                            // north
                            //                            if (bin1[i1*2+1] > 0) {
                            //                                getrgb(bin1[i1*2], bin1[i1*2+1] - 1, tmp1, rgb);
                            //                                if (!isRed(rgb)) {
                            //                                    bin1[tail1*2] = bin1[i1*2];
                            //                                    bin1[tail1*2+1] = bin1[i1*2+1] - 1;
                            //                                    setredcolor(bin1[tail1*2], bin1[tail1*2+1], tmp1);
                            //                                    tail1 ++;
                            //                                }
                            //                            }
                            //                            // south
                            //                            if (bin1[i1*2+1] < height - 1) {
                            //                                getrgb(bin1[i1*2], bin1[i1*2+1] + 1, tmp1, rgb);
                            //                                if (!isRed(rgb)) {
                            //                                    bin1[tail1*2] = bin1[i1*2];
                            //                                    bin1[tail1*2+1] = bin1[i1*2+1] + 1;
                            //                                    setredcolor(bin1[tail1*2], bin1[tail1*2+1], tmp1);
                            //                                    tail1 ++;
                            //                                }
                            //                            }
                            //                            // west
                            //                            if (bin1[i1*2] > 0) {
                            //                                getrgb(bin1[i1*2] - 1, bin1[i1*2+1], tmp1, rgb);
                            //                                if (!isRed(rgb)) {
                            //                                    bin1[tail1*2] = bin1[i1*2] - 1;
                            //                                    bin1[tail1*2+1] = bin1[i1*2+1];
                            //                                    setredcolor(bin1[tail1*2], bin1[tail1*2+1], tmp1);
                            //                                    tail1 ++;
                            //                                }
                            //                            }
                            //                            // east
                            //                            if (bin1[i1*2] < width - 1) {
                            //                                getrgb(bin1[i1*2] + 1, bin1[i1*2+1], tmp1, rgb);
                            //                                if (!isRed(rgb)) {
                            //                                    bin1[tail1*2] = bin1[i1*2] + 1;
                            //                                    bin1[tail1*2+1] = bin1[i1*2+1];
                            //                                    setredcolor(bin1[tail1*2], bin1[tail1*2+1], tmp1);
                            //                                    tail1 ++;
                            //                                }
                            //                            }
                            //                            // north e
                            //                            if (bin1[i1*2+1] > 0 && bin1[i1*2] < width - 1) {
                            //                                getrgb(bin1[i1*2] + 1, bin1[i1*2+1] - 1, tmp1, rgb);
                            //                                if (!isRed(rgb)) {
                            //                                    bin1[tail1*2] = bin1[i1*2];
                            //                                    bin1[tail1*2+1] = bin1[i1*2+1] - 1;
                            //                                    setredcolor(bin1[tail1*2], bin1[tail1*2+1], tmp1);
                            //                                    tail1 ++;
                            //                                }
                            //                            }
                            //                            // south w
                            //                            if (bin1[i1*2+1] < height - 1 && bin1[i1*2] > 0) {
                            //                                getrgb(bin1[i1*2] - 1, bin1[i1*2+1] + 1, tmp1, rgb);
                            //                                if (!isRed(rgb)) {
                            //                                    bin1[tail1*2] = bin1[i1*2];
                            //                                    bin1[tail1*2+1] = bin1[i1*2+1] + 1;
                            //                                    setredcolor(bin1[tail1*2], bin1[tail1*2+1], tmp1);
                            //                                    tail1 ++;
                            //                                }
                            //                            }
                            //                            // n west
                            //                            if (bin1[i1*2] > 0 && bin1[i1*2+1] > 0) {
                            //                                getrgb(bin1[i1*2] - 1, bin1[i1*2+1] - 1, tmp1, rgb);
                            //                                if (!isRed(rgb)) {
                            //                                    bin1[tail1*2] = bin1[i1*2] - 1;
                            //                                    bin1[tail1*2+1] = bin1[i1*2+1];
                            //                                    setredcolor(bin1[tail1*2], bin1[tail1*2+1], tmp1);
                            //                                    tail1 ++;
                            //                                }
                            //                            }
                            //                            // s east
                            //                            if (bin1[i1*2] < width - 1 && bin1[i1*2+1] < height - 1) {
                            //                                getrgb(bin1[i1*2] + 1, bin1[i1*2+1] + 1, tmp1, rgb);
                            //                                if (!isRed(rgb)) {
                            //                                    bin1[tail1*2] = bin1[i1*2] + 1;
                            //                                    bin1[tail1*2+1] = bin1[i1*2+1];
                            //                                    setredcolor(bin1[tail1*2], bin1[tail1*2+1], tmp1);
                            //                                    tail1 ++;
                            //                                }
                            //                            }
                        }
                        head1 ++;
                    }
                }
                //            cout << tail1 << " ";
                if (((maxx - minx > 200) || (maxy - miny > 200))) {
                    for (int i1 = 0; i1 < tail1; i1 ++) {
                        setblackcolor(bin1[i1*2], bin1[i1*2+1], dst);
                    }
                }
            }
            
            
            for (int i = 0; i < tail; i ++) {
                if (bin[i*2] < 10 || bin[i*2] > width - 10 || bin[i*2+1] < 10 || bin[i*2+1] > height - 10) {
                    tail --;
                    bin[i*2] = bin[tail*2];
                    bin[i*2+1] = bin[tail*2+1];
                    continue;
                }
                uchar *thisrgb = new uchar[3];
                getrgb(bin[i*2], bin[i*2+1], dst, thisrgb);
                int avail = 0;
                for (int j = -5; j < 5; j ++) {
                    for (int k = -5; k < 5; k ++) {
                        if (j*j + k*k > 25) {
                            continue;
                        }
                        int x = bin[i*2] + j;
                        int y = bin[i*2+1] + k;
                        uchar *rgb = new uchar[3];
                        getrgb(x, y, dst, rgb);
                        if (isBlue(rgb)) {
                            avail ++;
                        }
                    }
                }
                if (avail < 10) {
                    if (isBlue(thisrgb) || isRed(thisrgb)) {
                        setblackcolor(bin[i*2], bin[i*2+1], dst);
                    }
                    tail --;
                    bin[i*2] = bin[tail*2];
                    bin[i*2+1] = bin[tail*2+1];
                }
            }
            std::cout << tail << "\n";
            cvSaveImage("/Users/oyster/Desktop/temp5.bmp", dst);
            
            
            // #5 count
            std::cout << "final step\n";
            int count = 0;
            for (int i = 0; i < tail; i ++) {
                uchar *thisrgb = new uchar[3];
                getrgb(bin[i*2], bin[i*2+1], dst, thisrgb);
                if (isWhite(thisrgb)) {
                    continue;
                }
                double score = 0;
                int aver = 0;
                for (int a = 0; a < 360; a += 5) {
                    bool ok = false;
                    int bestr = 10;
                    for (int r = 0; r < 6; r ++) {
                        int x = bin[2*i]   + (10+r) * sin(a / M_PI);
                        int y = bin[2*i+1] + (10+r) * cos(a / M_PI);
                        uchar *rgb = new uchar[3];
                        getrgb(x, y, dst, rgb);
                        if (isBlue(rgb)) {
                            score += 1;
                            ok = true;
                            bestr = 10+r;
                            break;
                        }
                        x = bin[2*i]   + (10-r) * sin(a / M_PI);
                        y = bin[2*i+1] + (10-r) * cos(a / M_PI);
                        getrgb(x, y, dst, rgb);
                        if (isBlue(rgb)) {
                            score += 1;
                            ok = true;
                            bestr = 10-r;
                            break;
                        }
                    }
                    if (!ok) {
                        score -= 1;
                    }
                    aver += bestr;
                }
                aver /= 72;
                if (score > 30) {
                    int blackpts = 0;
                    int avlpts = 0;
                    for (int j = -aver-3; j < aver+3; j ++) {
                        for (int k = -aver-3; k < aver+3; k ++) {
                            if (j*j + k*k < 225) {
                                int x = bin[i*2] + j;
                                int y = bin[i*2+1] + k;
                                uchar *rgb2 = new uchar[3];
                                getrgb(x, y, dst, rgb2);
                                if (isBlack(rgb2) || isWhite(rgb2)) {
                                    blackpts ++;
                                }
                                if (isBlue(rgb2)) {
                                    avlpts ++;
                                }
                            }
                        }
                    }
                    if (blackpts > 50) {
                        continue;
                    }
                    //                    if (avlpts < aver * 4) {
                    //                        continue;
                    //                    }
                    count ++;
                    for (int j = -aver; j < aver; j ++) {
                        for (int k = -aver; k < aver; k ++) {
                            if (j*j + k*k < aver*aver) {
                                int x = bin[i*2] + j;
                                int y = bin[i*2+1] + k;
                                setwhitecolor(x, y, dst);                            
                            }
                        }
                    }
                }
            }
            string outputpath = "/Users/oyster/Desktop/out/";
            outputpath.append(file);
            std::cout << "saving to " << outputpath << "\n";
            cvSaveImage(outputpath.c_str(), dst);
            
            cvReleaseImage(&dst);
            std::cout << "done.\n";
            std::cout << count << "\n";
            fout << file << "\t" << count << "\n";
            fout.flush();
        }
    }
    fout.close();
    return 0;
}
