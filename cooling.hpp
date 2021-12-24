#ifndef __cooling_hpp__
#define __cooling_hpp__

namespace COP {

class Cooling {

  public:
    Cooling()
    : startTemp( 100 ), minTemp( 10 ), curTemp( 100 ), alpha( 0.995 ) {}
    Cooling( double startTemp, double minTemp, double alpha )
    : startTemp( startTemp ), minTemp( minTemp ),
      curTemp( startTemp ), alpha( alpha ) {}

    /** abstract method, to be defined */
    virtual Cooling& cool() = 0;

    void setStartTemperature( double temp ) {
      startTemp = temp;
      curTemp = temp;
    }

    void setMinTemperature( double temp ) {
      minTemp = temp;
    }

    bool isActive() const {
      return curTemp > minTemp;
    }

    double getTemperature() const {
      return curTemp;
    }

    Cooling& reset() {
      curTemp = startTemp;
      return *this;
    }

  protected:
    double startTemp;
    double minTemp;
    double curTemp;
    double alpha; 

};

class LinearCooling : public Cooling {

  public:
    using Cooling::Cooling;

  virtual Cooling& cool() {
    curTemp -= alpha;
    return *this;
  }

};

class GeometricCooling : public Cooling {

  public:
    using Cooling::Cooling;

  virtual Cooling& cool() {
    curTemp *= alpha;
    return *this;
  }

};

};

#endif /* __cooler_hpp__ */
