## E-STORE

E-STORE is  a real-time image sharing system. E-STORE consists of an Android APP installed on the smartphones and a remote server in cloud.

### Server
--
The server is written in C language in a Linux environment. The server uses [flann](http://www.cs.ubc.ca/research/flann/) for the kd-tree match. The index structure in  the server is [Nest](https://github.com/rightpeter/DSSE.git). The image library is in the folder ./trace/image_keypoint.

### Client
--
The client is written in Java in an Android environment. The client uses [SIFT](http://en.wikipedia.org/wiki/Scale-invariant_feature_transform) algorithm to extrat the local features of images. 

### Usage
--
The usage of the server:
  - make
  - ./main

The usage of the client:
  - Set your server's IP address in ReceiveFive.java, uploadFeatures.java and UploadPicActivity.java.
  - Install the app into your mobile phone.

### Contact
--
Email:  pfzuo@hust.edu.cn
If you have any questions, please feel free to contact me.
