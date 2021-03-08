#include "pid.h"


PID::PID(double p, double i, double d) {
  _defaultPGain = _pGain = p;
  _defaultIGain = _iGain = i;
  _defaultDGain = _dGain = d;
};

void PID::setPIDGains(double p, double i, double d) {
  _pGain = p;
  _iGain = i;
  _dGain = d;
};

void PID::setPIDGainsToDefault(void) {
  _pGain = _defaultPGain;
  _iGain = _defaultIGain;
  _dGain = _defaultDGain;
};

double PID::calPIDValue(double i) {
  _input = i;  
  return calPIDValue();
};

double PID::calPIDValue(void) {
  _error = _target - _input;
  
  _pValue = _error;
  _iValue += (_error) * _interval;
  _dValue = (_error - _pError) / _interval;
  
  _PIDValue = _pValue * _pGain + _iValue * _iGain + _dValue * _dGain;

  _pError = _error;
  
  return _PIDValue;
};