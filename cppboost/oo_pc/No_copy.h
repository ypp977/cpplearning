#ifndef __NO_COPY_H__
#define __NO_COPY_H__
class No_copy
{
protected:
    No_copy();
    ~No_copy();
    No_copy(const No_copy &rhs) = delete;
    No_copy &operator=(const No_copy &rhs) = delete;

private:
};
#endif