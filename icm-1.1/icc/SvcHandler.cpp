
#include "icc/SvcHandler.h"
#include "icc/Reactor.h"

SvcHandler::SvcHandler(ThreadManager* thrMan, Reactor* reactor)
: Thread (thrMan)
{
  this->setReactor(reactor);
}

SvcHandler::~SvcHandler() {
  if (this->getReactor ()) {
      this->getReactor ()->removeHandler(this, EventHandler::ALL_EVENTS_MASK | EventHandler::DONT_CALL);
  }

//  OS::shutdown(stream.getHandle(), SHUT_RDWR);
  stream.close();
}

int
SvcHandler::open(void *)
{
  if (this->getReactor () &&
      this->getReactor ()->registerHandler (this, EventHandler::READ_MASK) == -1)
  {
  }

  return 0;
}

int
SvcHandler::registerWriteMask()
{
  if (this->getReactor () &&
      this->getReactor ()->registerHandler (this, EventHandler::WRITE_MASK) == -1)
  {
  }

  return 0;
}

int
SvcHandler::unRegisterWriteMask()
{
  if (this->getReactor () &&
      this->getReactor ()->removeHandler(this, EventHandler::WRITE_MASK | EventHandler::DONT_CALL))
  {
  }

  return 0;
}

SocketStream &
SvcHandler::getStream()
{
  return (SocketStream &) this->stream;
}

ACE_HANDLE
SvcHandler::getHandle() const
{
  return this->stream.getHandle ();
}

void
SvcHandler::setHandle(ACE_HANDLE h)
{
  this->stream.setHandle (h);
}

int
SvcHandler::handleClose(ACE_HANDLE , ReactorMask )
{
  this->destroy ();
  return 0;
}

void
SvcHandler::destroy ()
{
  delete this;
}
