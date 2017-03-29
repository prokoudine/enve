#ifndef IMAGESEQUENCEBOX_H
#define IMAGESEQUENCEBOX_H
#include "animationbox.h"

class ImageSequenceBox : public AnimationBox{
public:
    ImageSequenceBox(BoxesGroup *parent);
    void setListOfFrames(const QStringList &listOfFrames);
    BoundingBox *createNewDuplicate(BoxesGroup *parent);
    void loadUpdatePixmap();
    void reloadFile();
    void setUpdateVars();
    void prp_makeDuplicate(Property *targetBox);
private:
    QStringList mListOfFrames;
    QString mUpdateFramePath = "";
};

#endif // IMAGESEQUENCEBOX_H