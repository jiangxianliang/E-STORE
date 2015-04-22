## E-STORE

E-STORE is  a real-time image sharing system. E-STORE consists of an Android APP installed on the smartphones and a remote server in cloud.

### Server

The server  is written in C language in a Linux environment. Apart from this, the server uses [flann](http://www.cs.ubc.ca/research/flann/) for the kd-tree match.

### Usage

The usage of the server: `./main`
  - first the server build a image library
  - the small trace is under the `./trace/image_keypoint`. 
  - then the server is listening.

The usage of the client:
  - you need to use the eclipse to open the project, find out the server's ip address.
  - change the ip address in the cell phone client. 
  - install the app into your cellphone.

### Contact

If any questions, please contact me: pfzuo@hust.edu.cn  
Please feel free to contact me for details.
