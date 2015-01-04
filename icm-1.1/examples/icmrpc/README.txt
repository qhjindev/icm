1, compile and run:
   icc and icm should be compiled first for these examples to compile and run. That is, they depend on the icc and icm lib(libicc.so/libicc.a and libicm.so/libicm.a).

2, rpc client:
   There are two kinds of clients. Client.cpp shows the sync call and AmiClient.cpp shows the async call. The client call method is independent to the server dispath method. 
   That is, no matter the server uses AMD or not, the client can use both approach at will. 

3, rpc server:
   The server can also have two dispatch modes: sync or async(AMD). If use sync mode, one long time job may block other requests. So server usually uses the AMD mode.
   AmdServer.cpp shows the implementation of the Amd Server.
