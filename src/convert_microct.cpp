/*  Copyright (C) 2010 Imperial College London and others.
 *
 *  Please see the AUTHORS file in the main source directory for a
 *  full list of copyright holders.
 *
 *  Gerard Gorman
 *  Applied Modelling and Computation Group
 *  Department of Earth Science and Engineering
 *  Imperial College London
 *
 *  g.gorman@imperial.ac.uk
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  1. Redistributions of source code must retain the above copyright
 *  notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above
 *  copyright notice, this list of conditions and the following
 *  disclaimer in the documentation and/or other materials provided
 *  with the distribution.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 *  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
 *  BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 *  TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 *  ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 *  TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 *  THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 *  SUCH DAMAGE.
 */

#include <iostream>
#include <vector>
#include <string>
#include <getopt.h>
#include <cstring>

#include "CTImage.h"

void usage(char *cmd){
  std::cout<<"Usage: "<<cmd<<" CT-image [options]\n"
           <<"\nOptions:\n"
           <<" -h, --help\n\tHelp! Prints this message.\n"
           <<" -v, --verbose\n\tVerbose output.\n"
	   <<" -c format, --convert format\n\tConvert image to another format. Options are vox, inr, nhdr.\n"
           <<" -r resolution, --resolution resolution\n\tSet the image resolution in meters. This is useful when there is no meta-data.\n"
           <<" -x offset, --xoffset offset\n\tSpecify the offset along the x-axis when extracting a sub-block.\n"
           <<" -y offset, --yoffset offset\n\tSpecify the offset along the y-axis when extracting a sub-block.\n"
           <<" -z offset, --zoffset offset\n\tSpecify the offset along the z-axis when extracting a sub-block.\n"
           <<" -s width, --slab width\n\tExtract a square block of size 'width' from the data.\n";
  return;
}

int parse_arguments(int argc, char **argv,
                    std::string &filename, bool &verbose, std::string &convert, int offsets[], int &slab_width, double &resolution){

  // Set defaults
  verbose = false;
  slab_width = -1;
  resolution = -1;
  for(int i=0;i<3;i++)
    offsets[i] = 0;

  if(argc==1){
    usage(argv[0]);
    exit(0);
  }

  struct option longOptions[] = {
    {"help", 0, 0, 'h'},
    {"verbose", 0, 0, 'v'},
    {"convert", optional_argument, 0, 'c'},
    {"resolution", optional_argument, 0, 'r'},
    {"xoffset", optional_argument, 0, 'x'},
    {"yoffset", optional_argument, 0, 'y'},
    {"zoffset", optional_argument, 0, 'z'},
    {"slab", optional_argument, 0, 's'},
    {0, 0, 0, 0}
  };

  int optionIndex = 0;
  int verbosity = 0;
  int c;
  const char *shortopts = "hvc:r:s:x:y:z:";

  // Set opterr to nonzero to make getopt print error messages
  opterr=1;
  while (true){
    c = getopt_long(argc, argv, shortopts, longOptions, &optionIndex);
    
    if (c == -1) break;
    
    switch (c){
    case 'h':
      usage(argv[0]);
      break;
    case 'v':
      verbose = true;
      break;
    case 'c':
      convert = std::string(optarg);
      break;
    case 'r':
      resolution = atof(optarg);
      break;
    case 'x':
      offsets[0] = atoi(optarg);
      break;
    case 'y':
      offsets[1] = atoi(optarg);
      break;
    case 'z':
      offsets[2] = atoi(optarg);
      break;
    case 's':
      slab_width = atoi(optarg);
      break;
    case '?':
      // missing argument only returns ':' if the option string starts with ':'
      // but this seems to stop the printing of error messages by getopt?
      std::cerr<<"ERROR: unknown option or missing argument\n";
      usage(argv[0]);
      exit(-1);
    case ':':
      std::cerr<<"ERROR: missing argument\n";
      usage(argv[0]);
      exit(-1);
    default:
      // unexpected:
      std::cerr<<"ERROR: getopt returned unrecognized character code\n";
      exit(-1);
    }
  }

  filename = std::string(argv[argc-1]);

  return 0;
}

int main(int argc, char **argv){
  if(argc==1){
    usage(argv[0]);
    exit(-1);
  }
    
  std::string filename, convert;
  bool verbose, generate_mesh;
  int offsets[3], slab_width;
  double resolution;

  parse_arguments(argc, argv, filename, verbose, convert, offsets, slab_width, resolution);

  CTImage image;
  if(verbose)
    image.verbose_on();
  
  if(image.read(filename.c_str(), offsets, slab_width)<0){
    std::cerr<<"ERROR: Failed to read file."<<std::endl;
    exit(-1);
  }
  std::cout<<"INFO: Porosity of sample "<<image.get_porosity()<<std::endl;

  if(resolution>0){
    image.set_resolution(resolution);
  }

  if(convert==std::string("vox")){
    if(verbose)
      std::cout<<"INFO: Write VOX file\n";

    image.write_vox();
  }else if(convert==std::string("nhrd")){
    if(verbose)
      std::cout<<"INFO: Write NHDR file\n";

    image.write_nhdr();
  }else if(convert==std::string("inr")){
    if(verbose)
      std::cout<<"INFO: Write INR file\n";

    image.write_inr();
  }

  return 0;
}
