#include "qstringanimator.h"

QStringAnimator::QStringAnimator() : IntAnimator()
{

}

void QStringAnimator::setCurrentTextValue(const QString &text)
{
    int frame = getCurrentFrame();
    if(isKeyOnCurrentFrame()) {
        foreach(QStringKey *key, mTextValues) {
            if(key->keyFrame == frame) {
                key->string = text;
                break;
            }
        }
        setCurrentIntValue(frame, true);
    } else {
        mTextValues << new QStringKey(text, frame);
        setCurrentIntValue(frame, true);
    }
}

QString QStringAnimator::getCurrentTextValue()
{
    return getStringKeyWithLowerFrame(getCurrentFrame());
}

QString QStringAnimator::getStringKeyWithLowerFrame(int frame)
{
    QStringKey *bestKey = NULL;
    int dFrame = 10000000;
    foreach(QStringKey *key, mTextValues) {
        int newDFrame = frame - key->keyFrame;
        if(newDFrame == 0) {
            bestKey = key;
            dFrame = newDFrame;
            break;
        }
        if((dFrame < 0 && newDFrame > dFrame) || (dFrame > 0 && newDFrame < dFrame && newDFrame > 0) || bestKey == NULL) {
            dFrame = newDFrame;
            bestKey = key;
        }
    }
    if(bestKey == NULL) {
        return "";
    }
    return bestKey->string;
}