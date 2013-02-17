#ifndef VESTESTDRIVER_H
#define VESTESTDRIVER_H

template <typename AppT>
class vesTestDriver
{
public:
  vesTestDriver(AppT* app) :
    m_test(app)
  {
  }

  virtual int init() = 0;
  virtual void finalize() = 0;
  virtual void render() = 0;
  virtual void start() = 0;

protected:
  AppT* m_test;
};

#endif // VESTESTDRIVER_H
