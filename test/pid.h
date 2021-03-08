#ifndef _PID_H
#define _PID_H

class PID {
public:
  PID(double p, double i, double d);
  void setPIDGains(double p, double i, double d);
  void setPIDGainsToDefault(void);
  void setPIDMaxValue(double m) {_maxValue = m;};
  void setPIDValue(double pid) {_PIDValue = pid;};
  void setTarget(double t) {_target = t;};
  void setInput(double i) {_input = i;};
  void setInterval(double i) {_interval = i;};
  void clsPValue(void) {_pValue = 0;};
  void clsIValue(void) {_iValue = 0;};
  void clsDValue(void) {_dValue = 0;};
  void clsPIDValues(void) {_pValue = _iValue = _dValue = 0;};
  double calPIDValue(double i);
  double calPIDValue(void);
  double getPIDValue(void) {return _PIDValue;};
  double _target;
  
private:
  double _pGain,
          _iGain,
          _dGain;
  
  double _defaultPGain,
          _defaultIGain,
          _defaultDGain;

  double _maxValue = 200.0;
  double _PIDValue;

  double _pValue = 0,
          _iValue = 0,
          _dValue = 0;
  double _input;
  double _error, _pError;
  
  double _interval = 0.01;
};


#endif