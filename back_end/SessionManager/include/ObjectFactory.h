// Copyright (C) 1998 The New York Group Theory Cooperative
// See magnus/doc/COPYRIGHT for the full notice.

// Contents: Implementation of class 
//
// Principal Author: Dmitry Pechkin
//
// Status: in progress
//
// Revision History:
//

//#include "SMObject.h"

class ObjectFactory {
public:
  virtual class SMObject* factory() const = 0;
};

class SymmetricGroupFactory : public ObjectFactory {
public:
  SymmetricGroupFactory( int num ) : n(num) {}
  class SMObject* factory() const;
private:
  int n;
};
