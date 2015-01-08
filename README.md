icm
===

ICM(internet communication manager), refactored from ACE/TAO/ICE, is a high performance, lightweight distributed communication framework that support the development of concurrency, realtime and distributed applications using c++/java language.

  The framework include three major components:

   ICM(internet communication manager): a framework that support multiple protocols, heterogeneous environment:
    support pub/sub message and rpc-style remote invocation
    Support pluggable transport protocol implementations. 
    Support client side synch/asynch method invocation
    Support server side synch/asynch method dispatch
    Support efficient data marshal/demarshal..

   ICC(internet communication controller): cross-platform framework for high performance communication
The communication framework encapsulate facilities for cross-platform operating system processes / threads, network, 
virtual memory, event distribution, timers and message queues and other, and the use of design patterns and 
hierarchical design ideas, to develop a set of portable reusable application development framework. 
The framework can used to developting concurrent, real-time distributed systems.

   CodeGenerator: used to support the generate data marshal/demarshal code and client/server transport processing code.
    support cross platform code generator
    support an interface description language(idl) syntax
    rewriten from icee-trans
 
