/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QIMAGE_H
#define QIMAGE_H

#include <QtGui/qtransform.h>
#include <QtGui/qpaintdevice.h>
#include <QtGui/qrgb.h>
#include <QtCore/qbytearray.h>
#include <QtCore/qrect.h>
#include <QtCore/qstring.h>

#if QT_DEPRECATED_SINCE(5, 0)
#include <QtCore/qstringlist.h>
#endif

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

class QIODevice;
class QStringList;
class QMatrix;
class QTransform;
class QVariant;
template <class T> class QList;
template <class T> class QVector;

struct QImageData;
class QImageDataMisc; // internal
#ifndef QT_NO_IMAGE_TEXT
#if QT_DEPRECATED_SINCE(5, 0)
class Q_GUI_EXPORT QImageTextKeyLang {
public:
    QImageTextKeyLang(const char* k, const char* l) : key(k), lang(l) { }
    QImageTextKeyLang() { }

    QByteArray key;
    QByteArray lang;

    bool operator< (const QImageTextKeyLang& other) const
        { return key < other.key || (key==other.key && lang < other.lang); }
    bool operator== (const QImageTextKeyLang& other) const
        { return key==other.key && lang==other.lang; }
    inline bool operator!= (const QImageTextKeyLang &other) const
        { return !operator==(other); }
};
#endif
#endif //QT_NO_IMAGE_TEXT


class Q_GUI_EXPORT QImage : public QPaintDevice
{
public:
    enum InvertMode { InvertRgb, InvertRgba };
    enum Format {
        Format_Invalid,
        Format_Mono,
        Format_MonoLSB,
        Format_Indexed8,
        Format_RGB32,
        Format_ARGB32,
        Format_ARGB32_Premultiplied,
        Format_RGB16,
        Format_ARGB8565_Premultiplied,
        Format_RGB666,
        Format_ARGB6666_Premultiplied,
        Format_RGB555,
        Format_ARGB8555_Premultiplied,
        Format_RGB888,
        Format_RGB444,
        Format_ARGB4444_Premultiplied,
#if 0
        // reserved for future use
        Format_RGB15,
        Format_Grayscale16,
        Format_Grayscale8,
        Format_Grayscale4,
        Format_Grayscale4LSB,
        Format_Grayscale2,
        Format_Grayscale2LSB
#endif
#ifndef qdoc
        NImageFormats
#endif
    };

    QImage();
    QImage(const QSize &size, Format format);
    QImage(int width, int height, Format format);
    QImage(uchar *data, int width, int height, Format format);
    QImage(const uchar *data, int width, int height, Format format);
    QImage(uchar *data, int width, int height, int bytesPerLine, Format format);
    QImage(const uchar *data, int width, int height, int bytesPerLine, Format format);

#ifndef QT_NO_IMAGEFORMAT_XPM
    explicit QImage(const char * const xpm[]);
#endif
    explicit QImage(const QString &fileName, const char *format = 0);

    QImage(const QImage &);
    ~QImage();

    QImage &operator=(const QImage &);
#ifdef Q_COMPILER_RVALUE_REFS
    inline QImage &operator=(QImage &&other)
    { qSwap(d, other.d); return *this; }
#endif
    inline void swap(QImage &other) { qSwap(d, other.d); }

    bool isNull() const;

    int devType() const;

    bool operator==(const QImage &) const;
    bool operator!=(const QImage &) const;
    operator QVariant() const;
    void detach();
    bool isDetached() const;

    QImage copy(const QRect &rect = QRect()) const;
    inline QImage copy(int x, int y, int w, int h) const
        { return copy(QRect(x, y, w, h)); }

    Format format() const;

    QImage convertToFormat(Format f, Qt::ImageConversionFlags flags = Qt::AutoColor) const Q_REQUIRED_RESULT;
    QImage convertToFormat(Format f, const QVector<QRgb> &colorTable, Qt::ImageConversionFlags flags = Qt::AutoColor) const Q_REQUIRED_RESULT;

    int width() const;
    int height() const;
    QSize size() const;
    QRect rect() const;

    int depth() const;
    int colorCount() const;
    int bitPlaneCount() const;

    QRgb color(int i) const;
    void setColor(int i, QRgb c);
    void setColorCount(int);

    bool allGray() const;
    bool isGrayscale() const;

    uchar *bits();
    const uchar *bits() const;
    const uchar *constBits() const;

    int byteCount() const;

    uchar *scanLine(int);
    const uchar *scanLine(int) const;
    const uchar *constScanLine(int) const;
    int bytesPerLine() const;

    bool valid(int x, int y) const;
    bool valid(const QPoint &pt) const;

    int pixelIndex(int x, int y) const;
    int pixelIndex(const QPoint &pt) const;

    QRgb pixel(int x, int y) const;
    QRgb pixel(const QPoint &pt) const;

    void setPixel(int x, int y, uint index_or_rgb);
    void setPixel(const QPoint &pt, uint index_or_rgb);

    QVector<QRgb> colorTable() const;
    void setColorTable(const QVector<QRgb> colors);

    void fill(uint pixel);
    void fill(const QColor &color);
    void fill(Qt::GlobalColor color);


    bool hasAlphaChannel() const;
    void setAlphaChannel(const QImage &alphaChannel);
    QImage alphaChannel() const;
    QImage createAlphaMask(Qt::ImageConversionFlags flags = Qt::AutoColor) const;
#ifndef QT_NO_IMAGE_HEURISTIC_MASK
    QImage createHeuristicMask(bool clipTight = true) const;
#endif
    QImage createMaskFromColor(QRgb color, Qt::MaskMode mode = Qt::MaskInColor) const;

    inline QImage scaled(int w, int h, Qt::AspectRatioMode aspectMode = Qt::IgnoreAspectRatio,
                        Qt::TransformationMode mode = Qt::FastTransformation) const
        { return scaled(QSize(w, h), aspectMode, mode); }
    QImage scaled(const QSize &s, Qt::AspectRatioMode aspectMode = Qt::IgnoreAspectRatio,
                 Qt::TransformationMode mode = Qt::FastTransformation) const;
    QImage scaledToWidth(int w, Qt::TransformationMode mode = Qt::FastTransformation) const;
    QImage scaledToHeight(int h, Qt::TransformationMode mode = Qt::FastTransformation) const;
    QImage transformed(const QMatrix &matrix, Qt::TransformationMode mode = Qt::FastTransformation) const;
    static QMatrix trueMatrix(const QMatrix &, int w, int h);
    QImage transformed(const QTransform &matrix, Qt::TransformationMode mode = Qt::FastTransformation) const;
    static QTransform trueMatrix(const QTransform &, int w, int h);
    QImage mirrored(bool horizontally = false, bool vertically = true) const;
    QImage rgbSwapped() const;
    void invertPixels(InvertMode = InvertRgb);


    bool load(QIODevice *device, const char* format);
    bool load(const QString &fileName, const char* format=0);
    bool loadFromData(const uchar *buf, int len, const char *format = 0);
    inline bool loadFromData(const QByteArray &data, const char* aformat=0)
        { return loadFromData(reinterpret_cast<const uchar *>(data.constData()), data.size(), aformat); }

    bool save(const QString &fileName, const char* format=0, int quality=-1) const;
    bool save(QIODevice *device, const char* format=0, int quality=-1) const;

    static QImage fromData(const uchar *data, int size, const char *format = 0);
    inline static QImage fromData(const QByteArray &data, const char *format = 0)
        { return fromData(reinterpret_cast<const uchar *>(data.constData()), data.size(), format); }

    int serialNumber() const;
    qint64 cacheKey() const;

    QPaintEngine *paintEngine() const;

    // Auxiliary data
    int dotsPerMeterX() const;
    int dotsPerMeterY() const;
    void setDotsPerMeterX(int);
    void setDotsPerMeterY(int);
    QPoint offset() const;
    void setOffset(const QPoint&);
#ifndef QT_NO_IMAGE_TEXT
    QStringList textKeys() const;
    QString text(const QString &key = QString()) const;
    void setText(const QString &key, const QString &value);

#if QT_DEPRECATED_SINCE(5, 0)
    inline QString text(const char* key, const char* lang=0) const;
    inline QList<QImageTextKeyLang> textList() const;
    inline QStringList textLanguages() const;
    inline QString text(const QImageTextKeyLang&) const;
    inline void setText(const char* key, const char* lang, const QString&);
#endif
#endif

#if QT_DEPRECATED_SINCE(5, 0)
    QT_DEPRECATED inline int numColors() const;
    QT_DEPRECATED inline void setNumColors(int);
    QT_DEPRECATED inline int numBytes() const;
#endif

protected:
    virtual int metric(PaintDeviceMetric metric) const;

private:
    friend class QWSOnScreenSurface;
    QImageData *d;

    friend class QRasterPlatformPixmap;
    friend class QBlittablePlatformPixmap;
    friend class QPixmapCacheEntry;

public:
    typedef QImageData * DataPtr;
    inline DataPtr &data_ptr() { return d; }
};

Q_DECLARE_SHARED(QImage)
Q_DECLARE_TYPEINFO(QImage, Q_MOVABLE_TYPE);

// Inline functions...

Q_GUI_EXPORT_INLINE bool QImage::valid(const QPoint &pt) const { return valid(pt.x(), pt.y()); }
Q_GUI_EXPORT_INLINE int QImage::pixelIndex(const QPoint &pt) const { return pixelIndex(pt.x(), pt.y());}
Q_GUI_EXPORT_INLINE QRgb QImage::pixel(const QPoint &pt) const { return pixel(pt.x(), pt.y()); }
Q_GUI_EXPORT_INLINE void QImage::setPixel(const QPoint &pt, uint index_or_rgb) { setPixel(pt.x(), pt.y(), index_or_rgb); }

#if QT_DEPRECATED_SINCE(5, 0)
#ifndef QT_NO_IMAGE_TEXT
inline QString QImage::text(const char* key, const char* lang) const
{
    if (!d)
        return QString();
    QString k = QString::fromAscii(key);
    if (lang && *lang)
        k += QLatin1Char('/') + QString::fromAscii(lang);
    return text(k);
}

inline QList<QImageTextKeyLang> QImage::textList() const
{
    QList<QImageTextKeyLang> imageTextKeys;
    if (!d)
        return imageTextKeys;
    QStringList keys = textKeys();
    for (int i = 0; i < keys.size(); ++i) {
        int index = keys.at(i).indexOf(QLatin1Char('/'));
        if (index > 0) {
            QImageTextKeyLang tkl;
            tkl.key = keys.at(i).left(index).toAscii();
            tkl.lang = keys.at(i).mid(index+1).toAscii();
            imageTextKeys += tkl;
        }
    }

    return imageTextKeys;
}

inline QStringList QImage::textLanguages() const
{
    if (!d)
        return QStringList();
    QStringList keys = textKeys();
    QStringList languages;
    for (int i = 0; i < keys.size(); ++i) {
        int index = keys.at(i).indexOf(QLatin1Char('/'));
        if (index > 0)
            languages += keys.at(i).mid(index+1);
    }

    return languages;
}

inline QString QImage::text(const QImageTextKeyLang&kl) const
{
    if (!d)
        return QString();
    QString k = QString::fromAscii(kl.key.constData());
    if (!kl.lang.isEmpty())
        k += QLatin1Char('/') + QString::fromAscii(kl.lang.constData());
    return text(k);
}

inline void QImage::setText(const char* key, const char* lang, const QString &s)
{
    if (!d)
        return;
    detach();

    // In case detach() ran out of memory
    if (!d)
        return;

    QString k = QString::fromAscii(key);
    if (lang && *lang)
        k += QLatin1Char('/') + QString::fromAscii(lang);
    setText(k, s);
}
#endif
inline int QImage::numColors() const
{
    return colorCount();
}

inline void QImage::setNumColors(int n)
{
    setColorCount(n);
}

inline int QImage::numBytes() const
{
    return byteCount();
}
#endif

// QImage stream functions

#if !defined(QT_NO_DATASTREAM)
Q_GUI_EXPORT QDataStream &operator<<(QDataStream &, const QImage &);
Q_GUI_EXPORT QDataStream &operator>>(QDataStream &, QImage &);
#endif

QT_END_NAMESPACE

QT_END_HEADER

#endif // QIMAGE_H
