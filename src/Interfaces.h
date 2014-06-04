#ifndef _INTERFACES_H_
#define _INTERFACES_H_

class ProgressUpdatable
{
public:
	virtual void updateProgress(double value) = 0;
	virtual ~ProgressUpdatable() {}
};

#endif // _INTERFACES_H_
