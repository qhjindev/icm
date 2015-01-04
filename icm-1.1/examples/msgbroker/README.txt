 compile and run:

 icc and icm and icmmsg should be compiled first for these examples to compile and run. 
 That is, they depend on the icc, icm, and icmmsg lib(libicc.so/libicc.a, libicm.so/libicm.a and libicmmsg.so/libicmmsg.a).
 
 The icmmsg process shoud run first. Then start the Subscribe client to connect to icmmsg and subscribe topic.
 After that, run the Publish client to connect to icmmsg and publish messages of that topic. 
 Then the Subscribe client will receive that message.
