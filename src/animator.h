
#ifndef SRC_ANIMATOR_H_
#define SRC_ANIMATOR_H_

class Animator {
public:
    Animator() {};
    virtual ~Animator() {};
    virtual void reset() {};
    virtual void render() = 0;
};

#endif /* SRC_ANIMATOR_H_ */
