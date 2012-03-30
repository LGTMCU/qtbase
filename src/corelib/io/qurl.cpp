/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Copyright (C) 2012 Intel Corporation.
** Contact: http://www.qt-project.org/
**
** This file is part of the QtCore module of the Qt Toolkit.
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

/*!
    \class QUrl

    \brief The QUrl class provides a convenient interface for working
    with URLs.

    \reentrant
    \ingroup io
    \ingroup network
    \ingroup shared


    It can parse and construct URLs in both encoded and unencoded
    form. QUrl also has support for internationalized domain names
    (IDNs).

    The most common way to use QUrl is to initialize it via the
    constructor by passing a QString. Otherwise, setUrl() and
    setEncodedUrl() can also be used.

    URLs can be represented in two forms: encoded or unencoded. The
    unencoded representation is suitable for showing to users, but
    the encoded representation is typically what you would send to
    a web server. For example, the unencoded URL
    "http://b\uuml\c{}hler.example.com" would be sent to the server as
    "http://xn--bhler-kva.example.com/List%20of%20applicants.xml".

    A URL can also be constructed piece by piece by calling
    setScheme(), setUserName(), setPassword(), setHost(), setPort(),
    setPath(), setEncodedQuery() and setFragment(). Some convenience
    functions are also available: setAuthority() sets the user name,
    password, host and port. setUserInfo() sets the user name and
    password at once.

    Call isValid() to check if the URL is valid. This can be done at
    any point during the constructing of a URL.

    Constructing a query is particularly convenient through the use
    of setQueryItems(), addQueryItem() and removeQueryItem(). Use
    setQueryDelimiters() to customize the delimiters used for
    generating the query string.

    For the convenience of generating encoded URL strings or query
    strings, there are two static functions called
    fromPercentEncoding() and toPercentEncoding() which deal with
    percent encoding and decoding of QStrings.

    Calling isRelative() will tell whether or not the URL is
    relative. A relative URL can be resolved by passing it as argument
    to resolved(), which returns an absolute URL. isParentOf() is used
    for determining whether one URL is a parent of another.

    fromLocalFile() constructs a QUrl by parsing a local
    file path. toLocalFile() converts a URL to a local file path.

    The human readable representation of the URL is fetched with
    toString(). This representation is appropriate for displaying a
    URL to a user in unencoded form. The encoded form however, as
    returned by toEncoded(), is for internal use, passing to web
    servers, mail clients and so on.

    QUrl conforms to the URI specification from
    \l{RFC 3986} (Uniform Resource Identifier: Generic Syntax), and includes
    scheme extensions from \l{RFC 1738} (Uniform Resource Locators). Case
    folding rules in QUrl conform to \l{RFC 3491} (Nameprep: A Stringprep
    Profile for Internationalized Domain Names (IDN)).

    \section2 Character Conversions

    Follow these rules to avoid erroneous character conversion when
    dealing with URLs and strings:

    \list
    \li When creating an QString to contain a URL from a QByteArray or a
       char*, always use QString::fromUtf8().
    \endlist

    \sa QUrlInfo
*/

/*!
    \enum QUrl::ParsingMode

    The parsing mode controls the way QUrl parses strings.

    \value TolerantMode QUrl will try to correct some common errors in URLs.
                        This mode is useful when processing URLs entered by
                        users.

    \value StrictMode Only valid URLs are accepted. This mode is useful for
                      general URL validation.

    In TolerantMode, the parser corrects the following invalid input:

    \list

    \li Spaces and "%20": If an encoded URL contains a space, this will be
    replaced with "%20". If a decoded URL contains "%20", this will be
    replaced with a single space before the URL is parsed.

    \li Single "%" characters: Any occurrences of a percent character "%" not
    followed by exactly two hexadecimal characters (e.g., "13% coverage.html")
    will be replaced by "%25".

    \li Reserved and unreserved characters: An encoded URL should only
    contain a few characters as literals; all other characters should
    be percent-encoded. In TolerantMode, these characters will be
    automatically percent-encoded where they are not allowed:
            space / double-quote / "<" / ">" / "[" / "\" /
            "]" / "^" / "`" / "{" / "|" / "}"

    \endlist
*/

/*!
    \enum QUrl::FormattingOption

    The formatting options define how the URL is formatted when written out
    as text.

    \value None The format of the URL is unchanged.
    \value RemoveScheme  The scheme is removed from the URL.
    \value RemovePassword  Any password in the URL is removed.
    \value RemoveUserInfo  Any user information in the URL is removed.
    \value RemovePort      Any specified port is removed from the URL.
    \value RemoveAuthority
    \value RemovePath   The URL's path is removed, leaving only the scheme,
                        host address, and port (if present).
    \value RemoveQuery  The query part of the URL (following a '?' character)
                        is removed.
    \value RemoveFragment
    \value PreferLocalFile If the URL is a local file according to isLocalFile()
     and contains no query or fragment, a local file path is returned.
    \value StripTrailingSlash  The trailing slash is removed if one is present.

    Note that the case folding rules in \l{RFC 3491}{Nameprep}, which QUrl
    conforms to, require host names to always be converted to lower case,
    regardless of the Qt::FormattingOptions used.
*/

/*!
 \fn uint qHash(const QUrl &url)
 \since 4.7
 \relates QUrl

 Computes a hash key from the normalized version of \a url.
 */
#include "qurl.h"
#include "qurl_p.h"
#include "qplatformdefs.h"
#include "qstring.h"
#include "qstringlist.h"
#include "qdebug.h"
#include "qdir.h"         // for QDir::fromNativeSeparators
#include "qtldurl_p.h"
#include "private/qipaddress_p.h"
#include "qurlquery.h"
#if defined(Q_OS_WINCE_WM)
#pragma optimize("g", off)
#endif

QT_BEGIN_NAMESPACE

inline static bool isHex(char c)
{
    c |= 0x20;
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f');
}

static inline char toHex(quint8 c)
{
    return c > 9 ? c - 10 + 'A' : c + '0';
}

static inline QString ftpScheme()
{
    return QStringLiteral("ftp");
}

static inline QString httpScheme()
{
    return QStringLiteral("http");
}

static inline QString fileScheme()
{
    return QStringLiteral("file");
}

QUrlPrivate::QUrlPrivate()
    : ref(1), port(-1),
      errorCode(NoError), errorSupplement(0),
      sectionIsPresent(0), sectionHasError(0)
{
}

QUrlPrivate::QUrlPrivate(const QUrlPrivate &copy)
    : ref(1), port(copy.port),
      scheme(copy.scheme),
      userName(copy.userName),
      password(copy.password),
      host(copy.host),
      path(copy.path),
      query(copy.query),
      fragment(copy.fragment),
      errorCode(copy.errorCode),
      errorSupplement(copy.errorSupplement),
      sectionIsPresent(copy.sectionIsPresent),
      sectionHasError(copy.sectionHasError)
{
}

void QUrlPrivate::clear()
{
    scheme.clear();
    userName.clear();
    password.clear();
    host.clear();
    port = -1;
    path.clear();
    query.clear();
    fragment.clear();

    errorCode = NoError;
    errorSupplement = 0;
    sectionIsPresent = 0;
    sectionHasError = 0;
}


// From RFC 3896, Appendix A Collected ABNF for URI
//    URI           = scheme ":" hier-part [ "?" query ] [ "#" fragment ]
//[...]
//    scheme        = ALPHA *( ALPHA / DIGIT / "+" / "-" / "." )
//
//    authority     = [ userinfo "@" ] host [ ":" port ]
//    userinfo      = *( unreserved / pct-encoded / sub-delims / ":" )
//    host          = IP-literal / IPv4address / reg-name
//    port          = *DIGIT
//[...]
//    reg-name      = *( unreserved / pct-encoded / sub-delims )
//[..]
//    pchar         = unreserved / pct-encoded / sub-delims / ":" / "@"
//
//    query         = *( pchar / "/" / "?" )
//
//    fragment      = *( pchar / "/" / "?" )
//
//    pct-encoded   = "%" HEXDIG HEXDIG
//
//    unreserved    = ALPHA / DIGIT / "-" / "." / "_" / "~"
//    reserved      = gen-delims / sub-delims
//    gen-delims    = ":" / "/" / "?" / "#" / "[" / "]" / "@"
//    sub-delims    = "!" / "$" / "&" / "'" / "(" / ")"
//                  / "*" / "+" / "," / ";" / "="
// the path component has a complex ABNF that basically boils down to
// slash-separated segments of "pchar"

// The above is the strict definition of the URL components and it is what we
// return encoded as FullyEncoded. However, we store the equivalent to
// PrettyDecoded internally, as that is the default formatting mode and most
// likely to be used. PrettyDecoded decodes spaces, unicode sequences and
// unambiguous delimiters.
//
// An ambiguous delimiter is a delimiter that, if appeared decoded, would be
// interpreted as the beginning of a new component. From last to first
// component, they are:
//  - fragment: none, since it's the last.
//  - query: the "#" character is ambiguous, as it starts the fragment. In
//    addition, the "+" character is treated specially, as should be both
//    intra-query delimiters. Since we don't know which ones they are, we
//    keep all reserved characters untouched.
//  - path: the "#" and "?" characters are ambigous. In addition to them,
//    the slash itself is considered special.
//  - host: completely special, see setHost() below.
//  - password: the "#", "?", "/", and ":" characters are ambiguous
//  - username: the "#", "?", "/", ":", and "@" characters are ambiguous
//  - scheme: doesn't accept any delimiter, see setScheme() below.

// list the recoding table modifications to be used with the recodeFromUser
// function, according to the rules above

#define decode(x) ushort(x)
#define leave(x)  ushort(0x100 | (x))
#define encode(x) ushort(0x200 | (x))

static const ushort encodedUserNameActions[] = {
    // first field, everything must be encoded, including the ":"
    //    userinfo      = *( unreserved / pct-encoded / sub-delims / ":" )
    encode(':'), // 0
    encode('['), // 1
    encode(']'), // 2
    encode('@'), // 3
    encode('/'), // 4
    encode('?'), // 5
    encode('#'), // 6
    0
};
static const ushort * const prettyUserNameActions = encodedUserNameActions;
static const ushort * const decodedUserNameActions = 0;

static const ushort encodedPasswordActions[] = {
    // same as encodedUserNameActions, but decode ":"
    //    userinfo      = *( unreserved / pct-encoded / sub-delims / ":" )
    decode(':'), // 0
    encode('['), // 1
    encode(']'), // 2
    encode('@'), // 3
    encode('/'), // 4
    encode('?'), // 5
    encode('#'), // 6
    0
};
static const ushort * const prettyPasswordActions = encodedPasswordActions;
static const ushort * const decodedPasswordActions = 0;

static const ushort encodedPathActions[] = {
    //    pchar         = unreserved / pct-encoded / sub-delims / ":" / "@"
    encode('['), // 0
    encode(']'), // 1
    encode('?'), // 2
    encode('#'), // 3
    leave('/'),  // 4
    decode(':'), // 5
    decode('@'), // 6
    0
};
static const ushort * const prettyPathActions = encodedPathActions + 2; // allow decoding "[" / "]"
static const ushort * const decodedPathActions = encodedPathActions + 4; // equivalent to leave('/')

static const ushort encodedFragmentActions[] = {
    //    fragment      = *( pchar / "/" / "?" )
    // gen-delims permitted: ":" / "@" / "/" / "?"
    //   ->   must encode: "[" / "]" / "#"
    // HOWEVER: we allow "#" to remain decoded
    decode('#'), // 0
    decode(':'), // 1
    decode('@'), // 2
    decode('/'), // 3
    decode('?'), // 4
    encode('['), // 5
    encode(']'), // 6
    0
};
static const ushort * const prettyFragmentActions = 0;
static const ushort * const decodedFragmentActions = 0;

// the query is handled specially, since we prefer not to transform the delims
static const ushort * const encodedQueryActions = encodedFragmentActions + 4; // encode "#" / "[" / "]"


static inline QString
recodeFromUser(const QString &input, const ushort *actions, int from, int to)
{
    QString output;
    const QChar *begin = input.constData() + from;
    const QChar *end = input.constData() + to;
    if (qt_urlRecode(output, begin, end,
                     QUrl::DecodeUnicode | QUrl::DecodeAllDelimiters | QUrl::DecodeSpaces, actions))
        return output;

    return input.mid(from, to - from);
}

void QUrlPrivate::appendAuthority(QString &appendTo, QUrl::FormattingOptions options) const
{
    if ((options & QUrl::RemoveUserInfo) != QUrl::RemoveUserInfo) {
        appendUserInfo(appendTo, options);
        if (hasUserInfo())
            appendTo += QLatin1Char('@');
    }
    appendHost(appendTo, options);
    if (!(options & QUrl::RemovePort) && port != -1)
        appendTo += QLatin1Char(':') + QString::number(port);
}

void QUrlPrivate::appendUserInfo(QString &appendTo, QUrl::FormattingOptions options) const
{
    // when constructing the authority or user-info, we never encode the ambiguous delimiters
    options &= ~(QUrl::DecodeAllDelimiters & ~QUrl::DecodeUnambiguousDelimiters);

    appendUserName(appendTo, options);
    if (options & QUrl::RemovePassword || !hasPassword()) {
        return;
    } else {
        appendTo += QLatin1Char(':');
        appendPassword(appendTo, options);
    }
}

// appendXXXX functions:
// the internal value is already encoded in PrettyDecoded, so that case is easy.
// DecodeUnicode and DecodeSpaces are handled by qt_urlRecode.
// That leaves these functions to handle three cases related to delimiters:
//  1) encoded                           encodedXXXX tables
//  2) DecodeUnambiguousDelimiters       prettyXXXX tables
//  3) DecodeAllDelimiters               decodedXXXX tables
static inline void appendToUser(QString &appendTo, const QString &value, QUrl::FormattingOptions options,
                                const ushort *encodedActions, const ushort *prettyActions, const ushort *decodedActions)
{
    if (options == QUrl::PrettyDecoded) {
        appendTo += value;
        return;
    }

    const ushort *actions = 0;
    if ((options & QUrl::DecodeAllDelimiters) == QUrl::DecodeUnambiguousDelimiters) {
        actions = prettyActions;
    } else if (options & QUrl::DecodeAllDelimiters) {
        actions = decodedActions;
    } else if ((options & QUrl::DecodeAllDelimiters) == 0) {
        actions = encodedActions;
    }

    if (!qt_urlRecode(appendTo, value.constData(), value.constData() + value.length(),
                      options, actions))
        appendTo += value;
}

inline void QUrlPrivate::appendUserName(QString &appendTo, QUrl::FormattingOptions options) const
{
    appendToUser(appendTo, userName, options, encodedUserNameActions, prettyUserNameActions, decodedUserNameActions);
}

inline void QUrlPrivate::appendPassword(QString &appendTo, QUrl::FormattingOptions options) const
{
    appendToUser(appendTo, password, options, encodedPasswordActions, prettyPasswordActions, decodedPasswordActions);
}

inline void QUrlPrivate::appendPath(QString &appendTo, QUrl::FormattingOptions options) const
{
    appendToUser(appendTo, path, options, encodedPathActions, prettyPathActions, decodedPathActions);
}

inline void QUrlPrivate::appendFragment(QString &appendTo, QUrl::FormattingOptions options) const
{
    appendToUser(appendTo, fragment, options, encodedFragmentActions, prettyFragmentActions, decodedFragmentActions);
}

inline void QUrlPrivate::appendQuery(QString &appendTo, QUrl::FormattingOptions options) const
{
    // almost the same code as the previous functions
    // except we prefer not to touch the delimiters
    if (options == QUrl::PrettyDecoded) {
        appendTo += query;
        return;
    }

    const ushort *actions = 0;
    if ((options & QUrl::DecodeAllDelimiters) == QUrl::DecodeUnambiguousDelimiters) {
        // reset to default qt_urlRecode behaviour (leave delimiters alone)
        options &= ~QUrl::DecodeAllDelimiters;
    } else if ((options & QUrl::DecodeAllDelimiters) == 0) {
        actions = encodedQueryActions;
    }

    if (!qt_urlRecode(appendTo, query.constData(), query.constData() + query.length(),
                      options, actions))
        appendTo += query;
}

// setXXX functions

bool QUrlPrivate::setScheme(const QString &value, int len, bool decoded)
{
    // schemes are strictly RFC-compliant:
    //    scheme        = ALPHA *( ALPHA / DIGIT / "+" / "-" / "." )
    // but we need to decode any percent-encoding sequences that fall on
    // those characters
    // we also lowercase the scheme

    scheme.clear();
    sectionIsPresent |= Scheme;
    sectionHasError |= Scheme; // assume it has errors, we'll clear before returning true
    errorCode = SchemeEmptyError;
    if (len == 0)
        return false;

    // validate it:
    errorCode = InvalidSchemeError;
    int needsLowercasing = -1;
    const ushort *p = reinterpret_cast<const ushort *>(value.constData());
    for (int i = 0; i < len; ++i) {
        if (p[i] >= 'a' && p[i] <= 'z')
            continue;
        if (p[i] >= 'A' && p[i] <= 'Z') {
            needsLowercasing = i;
            continue;
        }
        if (p[i] >= '0' && p[i] <= '9' && i > 0)
            continue;
        if (p[i] == '+' || p[i] == '-' || p[i] == '.')
            continue;

        if (p[i] == '%') {
            // found a percent-encoded sign
            // if we haven't decoded yet, decode and try again
            errorSupplement = '%';
            if (decoded)
                return false;

            QString decodedScheme;
            if (qt_urlRecode(decodedScheme, value.constData(), value.constData() + len, 0, 0) == 0)
                return false;
            return setScheme(decodedScheme, decodedScheme.length(), true);
        }

        // found something else
        errorSupplement = p[i];
        return false;
    }

    scheme = value.left(len);
    sectionHasError &= ~Scheme;
    errorCode = NoError;

    if (needsLowercasing != -1) {
        // schemes are ASCII only, so we don't need the full Unicode toLower
        QChar *schemeData = scheme.data(); // force detaching here
        for (int i = needsLowercasing; i >= 0; --i) {
            register ushort c = schemeData[i].unicode();
            if (c >= 'A' && c <= 'Z')
                schemeData[i] = c + 0x20;
        }
    }
    return true;
}

bool QUrlPrivate::setAuthority(const QString &auth, int from, int end)
{
    sectionHasError &= ~Authority;
    sectionIsPresent &= ~Authority;
    sectionIsPresent |= Host;
    if (from == end) {
        userName.clear();
        password.clear();
        host.clear();
        port = -1;
        return true;
    }

    int userInfoIndex = auth.indexOf(QLatin1Char('@'), from);
    if (uint(userInfoIndex) < uint(end)) {
        setUserInfo(auth, from, userInfoIndex);
        from = userInfoIndex + 1;
    }

    int colonIndex = auth.lastIndexOf(QLatin1Char(':'), end - 1);
    if (colonIndex < from)
        colonIndex = -1;

    if (uint(colonIndex) < uint(end)) {
        if (auth.at(from).unicode() == '[') {
            // check if colonIndex isn't inside the "[...]" part
            int closingBracket = auth.indexOf(QLatin1Char(']'), from);
            if (uint(closingBracket) > uint(colonIndex))
                colonIndex = -1;
        }
    }

    if (colonIndex == end - 1) {
        // found a colon but no digits after it
        sectionHasError |= Port;
        errorCode = PortEmptyError;
    } else if (uint(colonIndex) < uint(end)) {
        unsigned long x = 0;
        for (int i = colonIndex + 1; i < end; ++i) {
            ushort c = auth.at(i).unicode();
            if (c >= '0' && c <= '9') {
                x *= 10;
                x += c - '0';
            } else {
                sectionHasError |= Port;
                errorCode = InvalidPortError;
                x = ulong(-1); // x != ushort(x)
                break;
            }
        }
        if (x == ushort(x)) {
            port = ushort(x);
        } else {
            sectionHasError |= Port;
            errorCode = InvalidPortError;
        }
    } else {
        port = -1;
    }

    return setHost(auth, from, qMin<uint>(end, colonIndex)) && !(sectionHasError & Port);
}

void QUrlPrivate::setUserInfo(const QString &userInfo, int from, int end)
{
    int delimIndex = userInfo.indexOf(QLatin1Char(':'), from);
    setUserName(userInfo, from, qMin<uint>(delimIndex, end));

    if (delimIndex == -1) {
        password.clear();
        sectionIsPresent &= ~Password;
        sectionHasError &= ~Password;
    } else {
        setPassword(userInfo, delimIndex + 1, end);
    }
}

inline void QUrlPrivate::setUserName(const QString &value, int from, int end)
{
    sectionIsPresent |= UserName;
    sectionHasError &= ~UserName;
    userName = recodeFromUser(value, prettyUserNameActions, from, end);
}

inline void QUrlPrivate::setPassword(const QString &value, int from, int end)
{
    sectionIsPresent |= Password;
    sectionHasError &= ~Password;
    password = recodeFromUser(value, prettyPasswordActions, from, end);
}

inline void QUrlPrivate::setPath(const QString &value, int from, int end)
{
    // sectionIsPresent |= Path; // not used, save some cycles
    sectionHasError &= ~Path;
    path = recodeFromUser(value, prettyPathActions, from, end);

    // ### FIXME?
    // check for the "path-noscheme" case
    // if the path contains a ":" before the first "/", it could be misinterpreted
    // as a scheme
}

inline void QUrlPrivate::setFragment(const QString &value, int from, int end)
{
    sectionIsPresent |= Fragment;
    sectionHasError &= ~Fragment;
    fragment = recodeFromUser(value, prettyFragmentActions, from, end);
}

inline void QUrlPrivate::setQuery(const QString &value, int from, int iend)
{
    sectionIsPresent |= Query;
    sectionHasError &= ~Query;

    // use the default actions for the query
    static const ushort decodeActions[] = {
        decode('"'),
        decode('<'),
        decode('>'),
        decode('\\'),
        decode('^'),
        decode('`'),
        decode('{'),
        decode('|'),
        decode('}'),
        encode('#'),
        0
    };
    QString output;
    const QChar *begin = value.constData() + from;
    const QChar *end = value.constData() + iend;
    if (qt_urlRecode(output, begin, end, QUrl::DecodeUnicode | QUrl::DecodeSpaces,
                     decodeActions))
        query = output;
    else
        query = value.mid(from, iend - from);
}

// Host handling
// The RFC says the host is:
//    host          = IP-literal / IPv4address / reg-name
//    IP-literal    = "[" ( IPv6address / IPvFuture  ) "]"
//    IPvFuture     = "v" 1*HEXDIG "." 1*( unreserved / sub-delims / ":" )
//  [a strict definition of IPv6Address and IPv4Address]
//     reg-name      = *( unreserved / pct-encoded / sub-delims )
//
// We deviate from the standard in all but IPvFuture. For IPvFuture we accept
// and store only exactly what the RFC says we should. No percent-encoding is
// permitted in this field, so Unicode characters and space aren't either.
//
// For IPv4 addresses, we accept broken addresses like inet_aton does (that is,
// less than three dots). However, we correct the address to the proper form
// and store the corrected address. After correction, we comply to the RFC and
// it's exclusively composed of unreserved characters.
//
// For IPv6 addresses, we accept addresses including trailing (embedded) IPv4
// addresses, the so-called v4-compat and v4-mapped addresses. We also store
// those addresses like that in the hostname field, which violates the spec.
// IPv6 hosts are stored with the square brackets in the QString. It also
// requires no transformation in any way.
//
// As for registered names, it's the other way around: we accept only valid
// hostnames as specified by STD 3 and IDNA. That means everything we accept is
// valid in the RFC definition above, but there are many valid reg-names
// according to the RFC that we do not accept in the name of security. Since we
// do accept IDNA, reg-names are subject to ACE encoding and decoding, which is
// specified by the DecodeUnicode flag. The hostname is stored in its Unicode form.

inline void QUrlPrivate::appendHost(QString &appendTo, QUrl::FormattingOptions options) const
{
    // this is the only flag that matters
    options &= QUrl::DecodeUnicode;
    if (host.isEmpty())
        return;
    if (host.at(0).unicode() == '[') {
        // IPv6Address and IPvFuture address never require any transformation
        appendTo += host;
    } else {
        // this is either an IPv4Address or a reg-name
        // if it is a reg-name, it is already stored in Unicode form
        if (options == QUrl::DecodeUnicode)
            appendTo += host;
        else
            appendTo += qt_ACE_do(host, ToAceOnly);
    }
}

// the whole IPvFuture is passed and parsed here, including brackets
static int parseIpFuture(QString &host, const QChar *begin, const QChar *end)
{
    //    IPvFuture     = "v" 1*HEXDIG "." 1*( unreserved / sub-delims / ":" )
    static const char acceptable[] =
            "!$&'()*+,;=" // sub-delims
            ":"           // ":"
            "-._~";       // unreserved

    // the brackets and the "v" have been checked
    if (begin[3].unicode() != '.')
        return begin[3].unicode();
    if ((begin[2].unicode() >= 'A' && begin[2].unicode() >= 'F') ||
            (begin[2].unicode() >= 'a' && begin[2].unicode() <= 'f') ||
            (begin[2].unicode() >= '0' && begin[2].unicode() <= '9')) {
        // this is so unlikely that we'll just go down the slow path
        // decode the whole string, skipping the "[vH." and "]" which we already know to be there
        host += QString::fromRawData(begin, 4);
        begin += 4;
        --end;

        QString decoded;
        if (qt_urlRecode(decoded, begin, end, QUrl::FullyEncoded, 0)) {
            begin = decoded.constBegin();
            end = decoded.constEnd();
        }

        for ( ; begin != end; ++begin) {
            if (begin->unicode() >= 'A' && begin->unicode() <= 'Z')
                host += *begin;
            else if (begin->unicode() >= 'a' && begin->unicode() <= 'z')
                host += *begin;
            else if (begin->unicode() >= '0' && begin->unicode() <= '9')
                host += *begin;
            else if (begin->unicode() < 0x80 && strchr(acceptable, begin->unicode()) != 0)
                host += *begin;
            else
                return begin->unicode();
        }
        host += QLatin1Char(']');
        return -1;
    }
    return begin[2].unicode();
}

// ONLY the IPv6 address is parsed here, WITHOUT the brackets
static bool parseIp6(QString &host, const QChar *begin, const QChar *end)
{
    QIPAddressUtils::IPv6Address address;
    if (!QIPAddressUtils::parseIp6(address, begin, end)) {
        // IPv6 failed parsing, check if it was a percent-encoded character in
        // the middle and try again
        QString decoded;
        if (!qt_urlRecode(decoded, begin, end, QUrl::FullyEncoded, 0)) {
            // no transformation, nothing to re-parse
            return false;
        }

        // recurse
        // if the parsing fails again, the qt_urlRecode above will return 0
        return parseIp6(host, decoded.constBegin(), decoded.constEnd());
    }

    host.reserve(host.size() + (end - begin));
    host += QLatin1Char('[');
    QIPAddressUtils::toString(host, address);
    host += QLatin1Char(']');
    return true;
}

bool QUrlPrivate::setHost(const QString &value, int from, int iend, bool maybePercentEncoded)
{
    const QChar *begin = value.constData() + from;
    const QChar *end = value.constData() + iend;

    const int len = end - begin;
    host.clear();
    sectionIsPresent |= Host;
    sectionHasError &= ~Host;
    if (len == 0)
        return true;

    if (begin[0].unicode() == '[') {
        // IPv6Address or IPvFuture
        // smallest IPv6 address is      "[::]"   (len = 4)
        // smallest IPvFuture address is "[v7.X]" (len = 6)
        if (end[-1].unicode() != ']') {
            sectionHasError |= Host;
            errorCode = HostMissingEndBracket;
            return false;
        }

        if (len > 5 && begin[1].unicode() == 'v') {
            int c = parseIpFuture(host, begin, end);
            if (c != -1) {
                sectionHasError |= Host;
                errorCode = InvalidIPvFutureError;
                errorSupplement = short(c);
            }
            return c == -1;
        }

        if (parseIp6(host, begin + 1, end - 1))
            return true;

        sectionHasError |= Host;
        errorCode = begin[1].unicode() == 'v' ?
                        InvalidIPvFutureError : InvalidIPv6AddressError;
        return false;
    }

    // check if it's an IPv4 address
    QIPAddressUtils::IPv4Address ip4;
    if (QIPAddressUtils::parseIp4(ip4, begin, end)) {
        // yes, it was
        QIPAddressUtils::toString(host, ip4);
        sectionHasError &= ~Host;
        return true;
    }

    // This is probably a reg-name.
    // But it can also be an encoded string that, when decoded becomes one
    // of the types above.
    //
    // Two types of encoding are possible:
    //  percent encoding (e.g., "%31%30%2E%30%2E%30%2E%31" -> "10.0.0.1")
    //  Unicode encoding (some non-ASCII characters case-fold to digits
    //                    when nameprepping is done)
    //
    // The qt_ACE_do function below applies nameprepping and the STD3 check.
    // That means a Unicode string may become an IPv4 address, but it cannot
    // produce a '[' or a '%'.

    // check for percent-encoding first
    QString s;
    if (maybePercentEncoded && qt_urlRecode(s, begin, end, QUrl::MostDecoded, 0)) {
        // something was decoded
        // anything encoded left?
        if (s.contains(QChar(0x25))) { // '%'
            sectionHasError |= Host;
            errorCode = InvalidRegNameError;
            return false;
        }

        // recurse
        return setHost(s, 0, s.length(), false);
    }

    s = qt_ACE_do(QString::fromRawData(begin, len), NormalizeAce);
    if (s.isEmpty()) {
        sectionHasError |= Host;
        errorCode = InvalidRegNameError;
        return false;
    }

    // check IPv4 again
    if (QIPAddressUtils::parseIp4(ip4, s.constBegin(), s.constEnd())) {
        QIPAddressUtils::toString(host, ip4);
    } else {
        host = s;
    }
    return true;
}

void QUrlPrivate::parse(const QString &url, QUrl::ParsingMode parsingMode)
{
    //   URI-reference = URI / relative-ref
    //   URI           = scheme ":" hier-part [ "?" query ] [ "#" fragment ]
    //   relative-ref  = relative-part [ "?" query ] [ "#" fragment ]
    //   hier-part     = "//" authority path-abempty
    //                 / other path types
    //   relative-part = "//" authority path-abempty
    //                 /  other path types here

    sectionIsPresent = 0;
    sectionHasError = 0;

    // find the important delimiters
    int colon = -1;
    int question = -1;
    int hash = -1;
    const int len = url.length();
    const QChar *const begin = url.constData();
    const ushort *const data = reinterpret_cast<const ushort *>(begin);

    for (int i = 0; i < len; ++i) {
        register uint uc = data[i];
        if (uc == '#' && hash == -1) {
            hash = i;

            // nothing more to be found
            break;
        }

        if (question == -1) {
            if (uc == ':' && colon == -1)
                colon = i;
            else if (uc == '?')
                question = i;
        }
    }

    // check if we have a scheme
    int hierStart;
    if (colon != -1 && setScheme(url, colon)) {
        hierStart = colon + 1;
    } else {
        // recover from a failed scheme: it might not have been a scheme at all
        scheme.clear();
        sectionHasError = 0;
        sectionIsPresent = 0;
        hierStart = 0;
    }

    int pathStart;
    int hierEnd = qMin<uint>(qMin<uint>(question, hash), len);
    if (hierEnd - hierStart >= 2 && data[hierStart] == '/' && data[hierStart + 1] == '/') {
        // we have an authority, it ends at the first slash after these
        int authorityEnd = hierEnd;
        for (int i = hierStart + 2; i < authorityEnd ; ++i) {
            if (data[i] == '/') {
                authorityEnd = i;
                break;
            }
        }

        setAuthority(url, hierStart + 2, authorityEnd);

        // even if we failed to set the authority properly, let's try to recover
        pathStart = authorityEnd;
        setPath(url, pathStart, hierEnd);
    } else {
        userName.clear();
        password.clear();
        host.clear();
        port = -1;
        pathStart = hierStart;

        if (hierStart < hierEnd)
            setPath(url, hierStart, hierEnd);
        else
            path.clear();
    }

    if (uint(question) < uint(hash))
        setQuery(url, question + 1, qMin<uint>(hash, len));

    if (hash != -1)
        setFragment(url, hash + 1, len);

    if (sectionHasError || parsingMode == QUrl::TolerantMode)
        return;

    // The parsing so far was tolerant of errors, so the StrictMode
    // parsing is actually implemented here, as an extra post-check.
    // We only execute it if we haven't found any errors so far.

    // What we need to look out for, that the regular parser tolerates:
    //  - percent signs not followed by two hex digits
    //  - forbidden characters, which should always appear encoded
    //    '"' / '<' / '>' / '\' / '^' / '`' / '{' / '|' / '}' / BKSP
    //    control characters
    //  - delimiters not allowed in certain positions
    //    . scheme: parser is already strict
    //    . user info: gen-delims (except for ':') disallowed
    //    . host: parser is stricter than the standard
    //    . port: parser is stricter than the standard
    //    . path: all delimiters allowed
    //    . fragment: all delimiters allowed
    //    . query: all delimiters allowed
    //    We would only need to check the user-info. However, the presence
    //    of the disallowed gen-delims changes the parsing, so we don't
    //    actually need to do anything
    static const char forbidden[] = "\"<>\\^`{|}\x7F";
    for (uint i = 0; i < uint(len); ++i) {
        register uint uc = data[i];
        if (uc >= 0x80)
            continue;

        if ((uc == '%' && (uint(len) < i + 2 || !isHex(data[i + 1]) || !isHex(data[i + 2])))
                || uc <= 0x20 || strchr(forbidden, uc)) {
            // found an error
            errorSupplement = uc;

            // where are we?
            if (i > uint(hash)) {
                errorCode = InvalidFragmentError;
                sectionHasError |= Fragment;
            } else if (i > uint(question)) {
                errorCode = InvalidQueryError;
                sectionHasError |= Query;
            } else if (i > uint(pathStart)) {
                // pathStart is never -1
                errorCode = InvalidPathError;
                sectionHasError |= Path;
            } else {
                // It must be in the authority, since the scheme is strict.
                // Since the port and hostname parsers are also strict,
                // the error can only have happened in the user info.
                int pos = url.indexOf(QLatin1Char(':'), hierStart);
                if (i > uint(pos)) {
                    errorCode = InvalidPasswordError;
                    sectionHasError |= Password;
                } else {
                    errorCode = InvalidUserNameError;
                    sectionHasError |= UserName;
                }
            }
        }
    }
}

/*
    From http://www.ietf.org/rfc/rfc3986.txt, 5.2.3: Merge paths

    Returns a merge of the current path with the relative path passed
    as argument.

    Note: \a relativePath is relative (does not start with '/').
*/
QString QUrlPrivate::mergePaths(const QString &relativePath) const
{
    // If the base URI has a defined authority component and an empty
    // path, then return a string consisting of "/" concatenated with
    // the reference's path; otherwise,
    if (!host.isEmpty() && path.isEmpty())
        return QLatin1Char('/') + relativePath;

    // Return a string consisting of the reference's path component
    // appended to all but the last segment of the base URI's path
    // (i.e., excluding any characters after the right-most "/" in the
    // base URI path, or excluding the entire base URI path if it does
    // not contain any "/" characters).
    QString newPath;
    if (!path.contains(QLatin1Char('/')))
        newPath = relativePath;
    else
        newPath = path.leftRef(path.lastIndexOf(QLatin1Char('/')) + 1) + relativePath;

    return newPath;
}

/*
    From http://www.ietf.org/rfc/rfc3986.txt, 5.2.4: Remove dot segments

    Removes unnecessary ../ and ./ from the path. Used for normalizing
    the URL.
*/
static void removeDotsFromPath(QString *path)
{
    // The input buffer is initialized with the now-appended path
    // components and the output buffer is initialized to the empty
    // string.
    QChar *out = path->data();
    const QChar *in = out;
    const QChar *end = out + path->size();

    // If the input buffer consists only of
    // "." or "..", then remove that from the input
    // buffer;
    if (path->size() == 1 && in[0].unicode() == '.')
        ++in;
    else if (path->size() == 2 && in[0].unicode() == '.' && in[1].unicode() == '.')
        in += 2;
    // While the input buffer is not empty, loop:
    while (in < end) {

        // otherwise, if the input buffer begins with a prefix of "../" or "./",
        // then remove that prefix from the input buffer;
        if (path->size() >= 2 && in[0].unicode() == '.' && in[1].unicode() == '/')
            in += 2;
        else if (path->size() >= 3 && in[0].unicode() == '.'
                 && in[1].unicode() == '.' && in[2].unicode() == '/')
            in += 3;

        // otherwise, if the input buffer begins with a prefix of
        // "/./" or "/.", where "." is a complete path segment,
        // then replace that prefix with "/" in the input buffer;
        if (in <= end - 3 && in[0].unicode() == '/' && in[1].unicode() == '.'
                && in[2].unicode() == '/') {
            in += 2;
            continue;
        } else if (in == end - 2 && in[0].unicode() == '/' && in[1].unicode() == '.') {
            *out++ = QLatin1Char('/');
            in += 2;
            break;
        }
        
        // otherwise, if the input buffer begins with a prefix
        // of "/../" or "/..", where ".." is a complete path
        // segment, then replace that prefix with "/" in the
        // input buffer and remove the last //segment and its
        // preceding "/" (if any) from the output buffer;
        if (in <= end - 4 && in[0].unicode() == '/' && in[1].unicode() == '.'
                && in[2].unicode() == '.' && in[3].unicode() == '/') {
            while (out > path->constData() && (--out)->unicode() != '/')
                ;
            if (out == path->constData() && out->unicode() != '/')
                ++in;
            in += 3;
            continue;
        } else if (in == end - 3 && in[0].unicode() == '/' && in[1].unicode() == '.'
                   && in[2].unicode() == '.') {
            while (out > path->constData() && (--out)->unicode() != '/')
                ;
            if (out->unicode() == '/')
                ++out;
            in += 3;
            break;
        }
        
        // otherwise move the first path segment in
        // the input buffer to the end of the output
        // buffer, including the initial "/" character
        // (if any) and any subsequent characters up
        // to, but not including, the next "/"
        // character or the end of the input buffer.
        *out++ = *in++;
        while (in < end && in->unicode() != '/')
            *out++ = *in++;
    }
    path->truncate(out - path->constData());
}

#if 0
void QUrlPrivate::validate() const
{
    QUrlPrivate *that = (QUrlPrivate *)this;
    that->encodedOriginal = that->toEncoded(); // may detach
    parse(ParseOnly);

    QURL_SETFLAG(that->stateFlags, Validated);

    if (!isValid)
        return;

    QString auth = authority(); // causes the non-encoded forms to be valid

    // authority() calls canonicalHost() which sets this
    if (!isHostValid)
        return;

    if (scheme == QLatin1String("mailto")) {
        if (!host.isEmpty() || port != -1 || !userName.isEmpty() || !password.isEmpty()) {
            that->isValid = false;
            that->errorInfo.setParams(0, QT_TRANSLATE_NOOP(QUrl, "expected empty host, username,"
                                                           "port and password"),
                                      0, 0);
        }
    } else if (scheme == ftpScheme() || scheme == httpScheme()) {
        if (host.isEmpty() && !(path.isEmpty() && encodedPath.isEmpty())) {
            that->isValid = false;
            that->errorInfo.setParams(0, QT_TRANSLATE_NOOP(QUrl, "the host is empty, but not the path"),
                                      0, 0);
        }
    }
}

const QByteArray &QUrlPrivate::normalized() const
{
    if (QURL_HASFLAG(stateFlags, QUrlPrivate::Normalized))
        return encodedNormalized;

    QUrlPrivate *that = const_cast<QUrlPrivate *>(this);
    QURL_SETFLAG(that->stateFlags, QUrlPrivate::Normalized);

    QUrlPrivate tmp = *this;
    tmp.scheme = tmp.scheme.toLower();
    tmp.host = tmp.canonicalHost();

    // ensure the encoded and normalized parts of the URL
    tmp.ensureEncodedParts();
    if (tmp.encodedUserName.contains('%'))
        q_normalizePercentEncoding(&tmp.encodedUserName, userNameExcludeChars);
    if (tmp.encodedPassword.contains('%'))
        q_normalizePercentEncoding(&tmp.encodedPassword, passwordExcludeChars);
    if (tmp.encodedFragment.contains('%'))
        q_normalizePercentEncoding(&tmp.encodedFragment, fragmentExcludeChars);

    if (tmp.encodedPath.contains('%')) {
        // the path is a bit special:
        // the slashes shouldn't be encoded or decoded.
        // They should remain exactly like they are right now
        //
        // treat the path as a slash-separated sequence of pchar
        QByteArray result;
        result.reserve(tmp.encodedPath.length());
        if (tmp.encodedPath.startsWith('/'))
            result.append('/');

        const char *data = tmp.encodedPath.constData();
        int lastSlash = 0;
        int nextSlash;
        do {
            ++lastSlash;
            nextSlash = tmp.encodedPath.indexOf('/', lastSlash);
            int len;
            if (nextSlash == -1)
                len = tmp.encodedPath.length() - lastSlash;
            else
                len = nextSlash - lastSlash;

            if (memchr(data + lastSlash, '%', len)) {
                // there's at least one percent before the next slash
                QByteArray block = QByteArray(data + lastSlash, len);
                q_normalizePercentEncoding(&block, pathExcludeChars);
                result.append(block);
            } else {
                // no percents in this path segment, append wholesale
                result.append(data + lastSlash, len);
            }

            // append the slash too, if it's there
            if (nextSlash != -1)
                result.append('/');

            lastSlash = nextSlash;
        } while (lastSlash != -1);

        tmp.encodedPath = result;
    }

    if (!tmp.scheme.isEmpty()) // relative test
        removeDotsFromPath(&tmp.encodedPath);

    int qLen = tmp.query.length();
    for (int i = 0; i < qLen; i++) {
        if (qLen - i > 2 && tmp.query.at(i) == '%') {
            ++i;
            tmp.query[i] = qToLower(tmp.query.at(i));
            ++i;
            tmp.query[i] = qToLower(tmp.query.at(i));
        }
    }
    encodedNormalized = tmp.toEncoded();

    return encodedNormalized;
}
#endif

/*!
    \macro QT_NO_URL_CAST_FROM_STRING
    \relates QUrl

    Disables automatic conversions from QString (or char *) to QUrl.

    Compiling your code with this define is useful when you have a lot of
    code that uses QString for file names and you wish to convert it to
    use QUrl for network transparency. In any code that uses QUrl, it can
    help avoid missing QUrl::resolved() calls, and other misuses of
    QString to QUrl conversions.

    \oldcode
        url = filename; // probably not what you want
    \newcode
        url = QUrl::fromLocalFile(filename);
        url = baseurl.resolved(QUrl(filename));
    \endcode

    \sa QT_NO_CAST_FROM_ASCII
*/


/*!
    Constructs a URL by parsing \a url. \a url is assumed to be in human
    readable representation, with no percent encoding. QUrl will automatically
    percent encode all characters that are not allowed in a URL.
    The default parsing mode is TolerantMode.

    Parses the \a url using the parser mode \a parsingMode.

    Example:

    \snippet doc/src/snippets/code/src_corelib_io_qurl.cpp 0

    To construct a URL from an encoded string, call fromEncoded():

    \snippet doc/src/snippets/code/src_corelib_io_qurl.cpp 1

    \sa setUrl(), setEncodedUrl(), fromEncoded(), TolerantMode
*/
QUrl::QUrl(const QString &url, ParsingMode parsingMode) : d(0)
{
    setUrl(url, parsingMode);
}

/*!
    Constructs an empty QUrl object.
*/
QUrl::QUrl() : d(0)
{
}

/*!
    Constructs a copy of \a other.
*/
QUrl::QUrl(const QUrl &other) : d(other.d)
{
    if (d)
        d->ref.ref();
}

/*!
    Destructor; called immediately before the object is deleted.
*/
QUrl::~QUrl()
{
    if (d && !d->ref.deref())
        delete d;
}

/*!
    Returns true if the URL is non-empty and valid; otherwise returns false.

    The URL is run through a conformance test. Every part of the URL
    must conform to the standard encoding rules of the URI standard
    for the URL to be reported as valid.

    \snippet doc/src/snippets/code/src_corelib_io_qurl.cpp 2
*/
bool QUrl::isValid() const
{
    if (isEmpty()) return false;
    return d->sectionHasError == 0;
}

/*!
    Returns true if the URL has no data; otherwise returns false.
*/
bool QUrl::isEmpty() const
{
    if (!d) return true;
    return d->isEmpty();
}

/*!
    Resets the content of the QUrl. After calling this function, the
    QUrl is equal to one that has been constructed with the default
    empty constructor.
*/
void QUrl::clear()
{
    if (d && !d->ref.deref())
        delete d;
    d = 0;
}

/*!
    Parses \a url using the parsing mode \a parsingMode.

    \a url is assumed to be in unicode format, with no percent
    encoding.

    Calling isValid() will tell whether or not a valid URL was constructed.

    \sa setEncodedUrl()
*/
void QUrl::setUrl(const QString &url, ParsingMode parsingMode)
{
    detach();
    d->parse(url, parsingMode);
}


/*!
    Sets the scheme of the URL to \a scheme. As a scheme can only
    contain ASCII characters, no conversion or encoding is done on the
    input.

    The scheme describes the type (or protocol) of the URL. It's
    represented by one or more ASCII characters at the start the URL,
    and is followed by a ':'. The following example shows a URL where
    the scheme is "ftp":

    \img qurl-authority2.png

    The scheme can also be empty, in which case the URL is interpreted
    as relative.

    \sa scheme(), isRelative()
*/
void QUrl::setScheme(const QString &scheme)
{
    detach();
    if (scheme.isEmpty()) {
        // schemes are not allowed to be empty
        d->sectionIsPresent &= ~QUrlPrivate::Scheme;
        d->sectionHasError &= ~QUrlPrivate::Scheme;
        d->scheme.clear();
    } else {
        d->setScheme(scheme, scheme.length());
    }
}

/*!
    Returns the scheme of the URL. If an empty string is returned,
    this means the scheme is undefined and the URL is then relative.

    \sa setScheme(), isRelative()
*/
QString QUrl::scheme() const
{
    if (!d) return QString();

    return d->scheme;
}

/*!
    Sets the authority of the URL to \a authority.

    The authority of a URL is the combination of user info, a host
    name and a port. All of these elements are optional; an empty
    authority is therefore valid.

    The user info and host are separated by a '@', and the host and
    port are separated by a ':'. If the user info is empty, the '@'
    must be omitted; although a stray ':' is permitted if the port is
    empty.

    The following example shows a valid authority string:

    \img qurl-authority.png
*/
void QUrl::setAuthority(const QString &authority)
{
    detach();
    d->setAuthority(authority, 0, authority.length());
    if (authority.isNull()) {
        // QUrlPrivate::setAuthority cleared almost everything
        // but it leaves the Host bit set
        d->sectionIsPresent &= ~QUrlPrivate::Authority;
    }
}

/*!
    Returns the authority of the URL if it is defined; otherwise
    an empty string is returned.

    \sa setAuthority()
*/
QString QUrl::authority(ComponentFormattingOptions options) const
{
    if (!d) return QString();

    QString result;
    d->appendAuthority(result, options);
    return result;
}

/*!
    Sets the user info of the URL to \a userInfo. The user info is an
    optional part of the authority of the URL, as described in
    setAuthority().

    The user info consists of a user name and optionally a password,
    separated by a ':'. If the password is empty, the colon must be
    omitted. The following example shows a valid user info string:

    \img qurl-authority3.png

    \sa userInfo(), setUserName(), setPassword(), setAuthority()
*/
void QUrl::setUserInfo(const QString &userInfo)
{
    detach();
    QString trimmed = userInfo.trimmed();
    d->setUserInfo(trimmed, 0, trimmed.length());
    if (userInfo.isNull()) {
        // QUrlPrivate::setUserInfo cleared almost everything
        // but it leaves the UserName bit set
        d->sectionIsPresent &= ~QUrlPrivate::UserInfo;
    }
}

/*!
    Returns the user info of the URL, or an empty string if the user
    info is undefined.
*/
QString QUrl::userInfo(ComponentFormattingOptions options) const
{
    if (!d) return QString();

    QString result;
    d->appendUserInfo(result, options);
    return result;
}

/*!
    Sets the URL's user name to \a userName. The \a userName is part
    of the user info element in the authority of the URL, as described
    in setUserInfo().

    \sa setEncodedUserName(), userName(), setUserInfo()
*/
void QUrl::setUserName(const QString &userName)
{
    detach();
    d->setUserName(userName, 0, userName.length());
    if (userName.isNull())
        d->sectionIsPresent &= ~QUrlPrivate::UserName;
}

/*!
    Returns the user name of the URL if it is defined; otherwise
    an empty string is returned.

    \sa setUserName(), encodedUserName()
*/
QString QUrl::userName(ComponentFormattingOptions options) const
{
    if (!d) return QString();

    QString result;
    d->appendUserName(result, options);
    return result;
}

/*!
    Sets the URL's password to \a password. The \a password is part of
    the user info element in the authority of the URL, as described in
    setUserInfo().

    \sa password(), setUserInfo()
*/
void QUrl::setPassword(const QString &password)
{
    detach();
    d->setPassword(password, 0, password.length());
    if (password.isNull())
        d->sectionIsPresent &= ~QUrlPrivate::Password;
}

/*!
    Returns the password of the URL if it is defined; otherwise
    an empty string is returned.

    \sa setPassword()
*/
QString QUrl::password(ComponentFormattingOptions options) const
{
    if (!d) return QString();

    QString result;
    d->appendPassword(result, options);
    return result;
}

/*!
    Sets the host of the URL to \a host. The host is part of the
    authority.

    \sa host(), setAuthority()
*/
void QUrl::setHost(const QString &host)
{
    detach();
    if (d->setHost(host, 0, host.length())) {
        if (host.isNull())
            d->sectionIsPresent &= ~QUrlPrivate::Host;
    } else if (!host.startsWith(QLatin1Char('['))) {
        // setHost failed, it might be IPv6 or IPvFuture in need of bracketing
        ushort oldCode = d->errorCode;
        ushort oldSupplement = d->errorSupplement;
        if (!d->setHost(QLatin1Char('[') + host + QLatin1Char(']'), 0, host.length() + 2)) {
            // failed again: choose if this was an IPv6 error or not
            if (!host.contains(QLatin1Char(':'))) {
                d->errorCode = oldCode;
                d->errorSupplement = oldSupplement;
            }
        }
    }
}

/*!
    Returns the host of the URL if it is defined; otherwise
    an empty string is returned.
*/
QString QUrl::host(ComponentFormattingOptions options) const
{
    if (!d) return QString();

    QString result;
    d->appendHost(result, options);
    if (result.startsWith(QLatin1Char('[')))
        return result.mid(1, result.length() - 2);
    return result;
}

/*!
    Sets the port of the URL to \a port. The port is part of the
    authority of the URL, as described in setAuthority().

    \a port must be between 0 and 65535 inclusive. Setting the
    port to -1 indicates that the port is unspecified.
*/
void QUrl::setPort(int port)
{
    detach();

    if (port < -1 || port > 65535) {
        qWarning("QUrl::setPort: Out of range");
        port = -1;
        d->sectionHasError |= QUrlPrivate::Port;
        d->errorCode = QUrlPrivate::InvalidPortError;
    } else {
        d->sectionHasError &= ~QUrlPrivate::Port;
    }

    d->port = port;
}

/*!
    \since 4.1

    Returns the port of the URL, or \a defaultPort if the port is
    unspecified.

    Example:

    \snippet doc/src/snippets/code/src_corelib_io_qurl.cpp 3
*/
int QUrl::port(int defaultPort) const
{
    if (!d) return defaultPort;
    return d->port == -1 ? defaultPort : d->port;
}

/*!
    Sets the path of the URL to \a path. The path is the part of the
    URL that comes after the authority but before the query string.

    \img qurl-ftppath.png

    For non-hierarchical schemes, the path will be everything
    following the scheme declaration, as in the following example:

    \img qurl-mailtopath.png

    \sa path()
*/
void QUrl::setPath(const QString &path)
{
    detach();
    d->setPath(path, 0, path.length());

    // optimized out, since there is no path delimiter
//    if (path.isNull())
//        d->sectionIsPresent &= ~QUrlPrivate::Path;
}

/*!
    Returns the path of the URL.

    \sa setPath()
*/
QString QUrl::path(ComponentFormattingOptions options) const
{
    if (!d) return QString();

    QString result;
    d->appendPath(result, options);
    return result;
}

/*!
    \since 4.2

    Returns true if this URL contains a Query (i.e., if ? was seen on it).

    \sa hasQueryItem(), encodedQuery()
*/
bool QUrl::hasQuery() const
{
    if (!d) return false;
    return d->hasQuery();
}

/*!
    Sets the query string of the URL to \a query. The string is
    inserted as-is, and no further encoding is performed when calling
    toEncoded().

    This function is useful if you need to pass a query string that
    does not fit into the key-value pattern, or that uses a different
    scheme for encoding special characters than what is suggested by
    QUrl.

    Passing a value of QByteArray() to \a query (a null QByteArray) unsets
    the query completely. However, passing a value of QByteArray("")
    will set the query to an empty value, as if the original URL
    had a lone "?".

    \sa encodedQuery(), hasQuery()
*/
void QUrl::setQuery(const QString &query)
{
    detach();

    d->setQuery(query, 0, query.length());
    if (query.isNull())
        d->sectionIsPresent &= ~QUrlPrivate::Query;
}

void QUrl::setQuery(const QUrlQuery &query)
{
    detach();

    // we know the data is in the right format
    d->query = query.toString();
    if (query.isEmpty())
        d->sectionIsPresent &= ~QUrlPrivate::Query;
    else
        d->sectionIsPresent |= QUrlPrivate::Query;
}

/*!
    Returns the query string of the URL in percent encoded form.
*/
QString QUrl::query(ComponentFormattingOptions options) const
{
    if (!d) return QString();

    QString result;
    d->appendQuery(result, options);
    if (d->hasQuery() && result.isNull())
        result.detach();
    return result;
}

/*!
    Sets the fragment of the URL to \a fragment. The fragment is the
    last part of the URL, represented by a '#' followed by a string of
    characters. It is typically used in HTTP for referring to a
    certain link or point on a page:

    \img qurl-fragment.png

    The fragment is sometimes also referred to as the URL "reference".

    Passing an argument of QString() (a null QString) will unset the fragment.
    Passing an argument of QString("") (an empty but not null QString)
    will set the fragment to an empty string (as if the original URL
    had a lone "#").

    \sa fragment(), hasFragment()
*/
void QUrl::setFragment(const QString &fragment)
{
    detach();

    d->setFragment(fragment, 0, fragment.length());
    if (fragment.isNull())
        d->sectionIsPresent &= ~QUrlPrivate::Fragment;
}

/*!
    Returns the fragment of the URL.

    \sa setFragment()
*/
QString QUrl::fragment(ComponentFormattingOptions options) const
{
    if (!d) return QString();

    QString result;
    d->appendFragment(result, options);
    if (d->hasFragment() && result.isNull())
        result.detach();
    return result;
}

/*!
    \since 4.2

    Returns true if this URL contains a fragment (i.e., if # was seen on it).

    \sa fragment(), setFragment()
*/
bool QUrl::hasFragment() const
{
    if (!d) return false;
    return d->hasFragment();
}

/*!
    \since 4.8

    Returns the TLD (Top-Level Domain) of the URL, (e.g. .co.uk, .net).
    Note that the return value is prefixed with a '.' unless the
    URL does not contain a valid TLD, in which case the function returns
    an empty string.
*/
QString QUrl::topLevelDomain(ComponentFormattingOptions options) const
{
    QString tld = qTopLevelDomain(host());
    if ((options & DecodeUnicode) == 0) {
        return qt_ACE_do(tld, ToAceOnly);
    }
    return tld;
}

/*!
    Returns the result of the merge of this URL with \a relative. This
    URL is used as a base to convert \a relative to an absolute URL.

    If \a relative is not a relative URL, this function will return \a
    relative directly. Otherwise, the paths of the two URLs are
    merged, and the new URL returned has the scheme and authority of
    the base URL, but with the merged path, as in the following
    example:

    \snippet doc/src/snippets/code/src_corelib_io_qurl.cpp 5

    Calling resolved() with ".." returns a QUrl whose directory is
    one level higher than the original. Similarly, calling resolved()
    with "../.." removes two levels from the path. If \a relative is
    "/", the path becomes "/".

    \sa isRelative()
*/
QUrl QUrl::resolved(const QUrl &relative) const
{
    if (!d) return relative;
    if (!relative.d) return *this;

    QUrl t;
    // be non strict and allow scheme in relative url
    if (!relative.d->scheme.isEmpty() && relative.d->scheme != d->scheme) {
        t = relative;
    } else {
        if (relative.d->hasAuthority()) {
            t = relative;
        } else {
            t.d = new QUrlPrivate;

            // copy the authority
            t.d->userName = d->userName;
            t.d->password = d->password;
            t.d->host = d->host;
            t.d->port = d->port;
            t.d->sectionIsPresent = d->sectionIsPresent & QUrlPrivate::Authority;

            if (relative.d->path.isEmpty()) {
                t.d->path = d->path;
                if (relative.d->hasQuery()) {
                    t.d->query = relative.d->query;
                    t.d->sectionIsPresent |= QUrlPrivate::Query;
                } else if (d->hasQuery()) {
                    t.d->query = d->query;
                    t.d->sectionIsPresent |= QUrlPrivate::Query;
                }
            } else {
                t.d->path = relative.d->path.startsWith(QLatin1Char('/'))
                            ? relative.d->path
                            : d->mergePaths(relative.d->path);
                if (relative.d->hasQuery()) {
                    t.d->query = relative.d->query;
                    t.d->sectionIsPresent |= QUrlPrivate::Query;
                }
            }
        }
        t.d->scheme = d->scheme;
        if (d->hasScheme())
            t.d->sectionIsPresent |= QUrlPrivate::Scheme;
        else
            t.d->sectionIsPresent &= ~QUrlPrivate::Scheme;
    }
    t.d->fragment = relative.d->fragment;
    if (relative.d->hasFragment())
        t.d->sectionIsPresent |= QUrlPrivate::Fragment;
    else
        t.d->sectionIsPresent &= ~QUrlPrivate::Fragment;

    removeDotsFromPath(&t.d->path);

#if defined(QURL_DEBUG)
    qDebug("QUrl(\"%s\").resolved(\"%s\") = \"%s\"",
           qPrintable(url()),
           qPrintable(relative.url()),
           qPrintable(t.url()));
#endif
    return t;
}

/*!
    Returns true if the URL is relative; otherwise returns false. A
    URL is relative if its scheme is undefined; this function is
    therefore equivalent to calling scheme().isEmpty().
*/
bool QUrl::isRelative() const
{
    if (!d) return true;
    return !d->hasScheme() && !d->path.startsWith(QLatin1Char('/'));
}

/*!
    Returns a string representation of the URL.
    The output can be customized by passing flags with \a options.

    The resulting QString can be passed back to a QUrl later on.

    Synonym for toString(options).

    \sa FormattingOptions, toEncoded(), toString()
*/
QString QUrl::url(FormattingOptions options) const
{
    return toString(options);
}

/*!
    Returns a string representation of the URL.
    The output can be customized by passing flags with \a options.

    \sa FormattingOptions, url(), setUrl()
*/
QString QUrl::toString(FormattingOptions options) const
{
    if (!d) return QString();

    // return just the path if:
    //  - QUrl::PreferLocalFile is passed
    //  - QUrl::RemovePath isn't passed (rather stupid if the user did...)
    //  - there's no query or fragment to return
    //    that is, either they aren't present, or we're removing them
    //  - it's a local file
    //    (test done last since it's the most expensive)
    if (options.testFlag(QUrl::PreferLocalFile) && !options.testFlag(QUrl::RemovePath)
            && (!d->hasQuery() || options.testFlag(QUrl::RemoveQuery))
            && (!d->hasFragment() || options.testFlag(QUrl::RemoveFragment))
            && isLocalFile()) {
        return path(options);
    }

    QString url;

    if (!(options & QUrl::RemoveScheme) && d->hasScheme())
        url += d->scheme + QLatin1Char(':');

    bool pathIsAbsolute = d->path.startsWith(QLatin1Char('/'));
    if (!((options & QUrl::RemoveAuthority) == QUrl::RemoveAuthority) && d->hasAuthority()) {
        url += QLatin1String("//");
        d->appendAuthority(url, options);
    } else if (isLocalFile() && pathIsAbsolute) {
        url += QLatin1String("//");
    }

    if (!(options & QUrl::RemovePath)) {
        // check if we need to insert a slash
        if (!pathIsAbsolute && !d->path.isEmpty() && !url.isEmpty() && !url.endsWith(QLatin1Char(':')))
            url += QLatin1Char('/');

        d->appendPath(url, options);
        // check if we need to remove trailing slashes
        while ((options & StripTrailingSlash) && url.endsWith(QLatin1Char('/')))
            url.chop(1);
    }

    if (!(options & QUrl::RemoveQuery) && d->hasQuery()) {
        url += QLatin1Char('?');
        d->appendQuery(url, options);
    }
    if (!(options & QUrl::RemoveFragment) && d->hasFragment()) {
        url += QLatin1Char('#');
        d->appendFragment(url, options);
    }

    return url;
}

/*!
    \since 5.0

    Returns a human-displayable string representation of the URL.
    The output can be customized by passing flags with \a options.
    The option RemovePassword is always enabled, since passwords
    should never be shown back to users.

    With the default options, the resulting QString can be passed back
    to a QUrl later on, but any password that was present initially will
    be lost.

    \sa FormattingOptions, toEncoded(), toString()
*/

QString QUrl::toDisplayString(FormattingOptions options) const
{
    return toString(options | RemovePassword);
}

/*!
    Returns the encoded representation of the URL if it's valid;
    otherwise an empty QByteArray is returned. The output can be
    customized by passing flags with \a options.

    The user info, path and fragment are all converted to UTF-8, and
    all non-ASCII characters are then percent encoded. The host name
    is encoded using Punycode.
*/
QByteArray QUrl::toEncoded(FormattingOptions options) const
{
    QString stringForm = toString(options);
    if (options & DecodeUnicode)
        return stringForm.toUtf8();
    return stringForm.toLatin1();
}

/*!
    \fn QUrl QUrl::fromEncoded(const QByteArray &input, ParsingMode parsingMode)

    Parses \a input and returns the corresponding QUrl. \a input is
    assumed to be in encoded form, containing only ASCII characters.

    Parses the URL using \a parsingMode.

    \sa toEncoded(), setUrl()
*/
QUrl QUrl::fromEncoded(const QByteArray &input, ParsingMode mode)
{
    return QUrl(QString::fromUtf8(input.constData(), input.size()), mode);
}

/*!
    Returns a decoded copy of \a input. \a input is first decoded from
    percent encoding, then converted from UTF-8 to unicode.
*/
QString QUrl::fromPercentEncoding(const QByteArray &input)
{
    return QString::fromUtf8(QByteArray::fromPercentEncoding(input));
}

/*!
    Returns an encoded copy of \a input. \a input is first converted
    to UTF-8, and all ASCII-characters that are not in the unreserved group
    are percent encoded. To prevent characters from being percent encoded
    pass them to \a exclude. To force characters to be percent encoded pass
    them to \a include.

    Unreserved is defined as:
       ALPHA / DIGIT / "-" / "." / "_" / "~"

    \snippet doc/src/snippets/code/src_corelib_io_qurl.cpp 6
*/
QByteArray QUrl::toPercentEncoding(const QString &input, const QByteArray &exclude, const QByteArray &include)
{
    return input.toUtf8().toPercentEncoding(exclude, include);
}

/*!
    \fn QByteArray QUrl::toPunycode(const QString &uc)
    \obsolete
    Returns a \a uc in Punycode encoding.

    Punycode is a Unicode encoding used for internationalized domain
    names, as defined in RFC3492. If you want to convert a domain name from
    Unicode to its ASCII-compatible representation, use toAce().
*/

/*!
    \fn QString QUrl::fromPunycode(const QByteArray &pc)
    \obsolete
    Returns the Punycode decoded representation of \a pc.

    Punycode is a Unicode encoding used for internationalized domain
    names, as defined in RFC3492. If you want to convert a domain from
    its ASCII-compatible encoding to the Unicode representation, use
    fromAce().
*/

/*!
    \since 4.2

    Returns the Unicode form of the given domain name
    \a domain, which is encoded in the ASCII Compatible Encoding (ACE).
    The result of this function is considered equivalent to \a domain.

    If the value in \a domain cannot be encoded, it will be converted
    to QString and returned.

    The ASCII Compatible Encoding (ACE) is defined by RFC 3490, RFC 3491
    and RFC 3492. It is part of the Internationalizing Domain Names in
    Applications (IDNA) specification, which allows for domain names
    (like \c "example.com") to be written using international
    characters.
*/
QString QUrl::fromAce(const QByteArray &domain)
{
    return qt_ACE_do(QString::fromLatin1(domain), NormalizeAce);
}

/*!
    \since 4.2

    Returns the ASCII Compatible Encoding of the given domain name \a domain.
    The result of this function is considered equivalent to \a domain.

    The ASCII-Compatible Encoding (ACE) is defined by RFC 3490, RFC 3491
    and RFC 3492. It is part of the Internationalizing Domain Names in
    Applications (IDNA) specification, which allows for domain names
    (like \c "example.com") to be written using international
    characters.

    This function return an empty QByteArra if \a domain is not a valid
    hostname. Note, in particular, that IPv6 literals are not valid domain
    names.
*/
QByteArray QUrl::toAce(const QString &domain)
{
    QString result = qt_ACE_do(domain, ToAceOnly);
    return result.toLatin1();
}

/*!
    \internal

    Returns true if this URL is "less than" the given \a url. This
    provides a means of ordering URLs.
*/
bool QUrl::operator <(const QUrl &url) const
{
    if (!d || !url.d) {
        bool thisIsEmpty = !d || d->isEmpty();
        bool thatIsEmpty = !url.d || url.d->isEmpty();

        // sort an empty URL first
        return thisIsEmpty && !thatIsEmpty;
    }

    int cmp;
    cmp = d->scheme.compare(url.d->scheme);
    if (cmp != 0)
        return cmp < 0;

    cmp = d->userName.compare(url.d->userName);
    if (cmp != 0)
        return cmp < 0;

    cmp = d->password.compare(url.d->password);
    if (cmp != 0)
        return cmp < 0;

    cmp = d->host.compare(url.d->host);
    if (cmp != 0)
        return cmp < 0;

    if (d->port != url.d->port)
        return d->port < url.d->port;

    cmp = d->path.compare(url.d->path);
    if (cmp != 0)
        return cmp < 0;

    cmp = d->query.compare(url.d->query);
    if (cmp != 0)
        return cmp < 0;

    cmp = d->fragment.compare(url.d->fragment);
    return cmp < 0;
}

/*!
    Returns true if this URL and the given \a url are equal;
    otherwise returns false.
*/
bool QUrl::operator ==(const QUrl &url) const
{
    if (!d && !url.d)
        return true;
    if (!d)
        return url.d->isEmpty();
    if (!url.d)
        return d->isEmpty();
    return d->scheme == url.d->scheme &&
            d->userName == url.d->userName &&
            d->password == url.d->password &&
            d->host == url.d->host &&
            d->port == url.d->port &&
            d->path == url.d->path &&
            d->query == url.d->query &&
            d->fragment == url.d->fragment;
}

/*!
    Returns true if this URL and the given \a url are not equal;
    otherwise returns false.
*/
bool QUrl::operator !=(const QUrl &url) const
{
    return !(*this == url);
}

/*!
    Assigns the specified \a url to this object.
*/
QUrl &QUrl::operator =(const QUrl &url)
{
    if (!d) {
        if (url.d) {
            url.d->ref.ref();
            d = url.d;
        }
    } else {
        if (url.d)
            qAtomicAssign(d, url.d);
        else
            clear();
    }
    return *this;
}

/*!
    Assigns the specified \a url to this object.
*/
QUrl &QUrl::operator =(const QString &url)
{
    if (url.isEmpty()) {
        clear();
    } else {
        detach();
        d->parse(url, TolerantMode);
    }
    return *this;
}

/*!
    \fn void QUrl::swap(QUrl &other)
    \since 4.8

    Swaps URL \a other with this URL. This operation is very
    fast and never fails.
*/

/*! \internal

    Forces a detach.
*/
void QUrl::detach()
{
    if (!d)
        d = new QUrlPrivate;
    else
        qAtomicDetach(d);
}

/*!
    \internal
*/
bool QUrl::isDetached() const
{
    return !d || d->ref.load() == 1;
}


/*!
    Returns a QUrl representation of \a localFile, interpreted as a local
    file. This function accepts paths separated by slashes as well as the
    native separator for this platform.

    This function also accepts paths with a doubled leading slash (or
    backslash) to indicate a remote file, as in
    "//servername/path/to/file.txt". Note that only certain platforms can
    actually open this file using QFile::open().

    \sa toLocalFile(), isLocalFile(), QDir::toNativeSeparators()
*/
QUrl QUrl::fromLocalFile(const QString &localFile)
{
    QUrl url;
    url.setScheme(fileScheme());
    QString deslashified = QDir::fromNativeSeparators(localFile);

    // magic for drives on windows
    if (deslashified.length() > 1 && deslashified.at(1) == QLatin1Char(':') && deslashified.at(0) != QLatin1Char('/')) {
        deslashified.prepend(QLatin1Char('/'));
    } else if (deslashified.startsWith(QLatin1String("//"))) {
        // magic for shared drive on windows
        int indexOfPath = deslashified.indexOf(QLatin1Char('/'), 2);
        url.setHost(deslashified.mid(2, indexOfPath - 2));
        if (indexOfPath > 2)
            deslashified = deslashified.right(deslashified.length() - indexOfPath);
        else
            deslashified.clear();
    }

    url.setPath(deslashified.replace(QLatin1Char('%'), QStringLiteral("%25")));
    return url;
}

/*!
    Returns the path of this URL formatted as a local file path. The path
    returned will use forward slashes, even if it was originally created
    from one with backslashes.

    If this URL contains a non-empty hostname, it will be encoded in the
    returned value in the form found on SMB networks (for example,
    "//servername/path/to/file.txt").

    \sa fromLocalFile(), isLocalFile()
*/
QString QUrl::toLocalFile() const
{
    // the call to isLocalFile() also ensures that we're parsed
    if (!isLocalFile())
        return QString();

    QString tmp;
    QString ourPath = path();

    // magic for shared drive on windows
    if (!d->host.isEmpty()) {
        tmp = QStringLiteral("//") + d->host + (ourPath.length() > 0 && ourPath.at(0) != QLatin1Char('/')
                                               ? QLatin1Char('/') + ourPath :  ourPath);
    } else {
        tmp = ourPath;
        // magic for drives on windows
        if (ourPath.length() > 2 && ourPath.at(0) == QLatin1Char('/') && ourPath.at(2) == QLatin1Char(':'))
            tmp.remove(0, 1);
    }

    return tmp;
}

/*!
    \since 4.7
    Returns true if this URL is pointing to a local file path. A URL is a
    local file path if the scheme is "file".

    Note that this function considers URLs with hostnames to be local file
    paths, even if the eventual file path cannot be opened with
    QFile::open().

    \sa fromLocalFile(), toLocalFile()
*/
bool QUrl::isLocalFile() const
{
    if (!d) return false;

    if (d->scheme != fileScheme())
        return false;   // not file
    return true;
}

/*!
    Returns true if this URL is a parent of \a childUrl. \a childUrl is a child
    of this URL if the two URLs share the same scheme and authority,
    and this URL's path is a parent of the path of \a childUrl.
*/
bool QUrl::isParentOf(const QUrl &childUrl) const
{
    QString childPath = childUrl.path();

    if (!d)
        return ((childUrl.scheme().isEmpty())
            && (childUrl.authority().isEmpty())
            && childPath.length() > 0 && childPath.at(0) == QLatin1Char('/'));

    QString ourPath = path();

    return ((childUrl.scheme().isEmpty() || d->scheme == childUrl.scheme())
            && (childUrl.authority().isEmpty() || authority() == childUrl.authority())
            &&  childPath.startsWith(ourPath)
            && ((ourPath.endsWith(QLatin1Char('/')) && childPath.length() > ourPath.length())
                || (!ourPath.endsWith(QLatin1Char('/'))
                    && childPath.length() > ourPath.length() && childPath.at(ourPath.length()) == QLatin1Char('/'))));
}


#ifndef QT_NO_DATASTREAM
/*! \relates QUrl

    Writes url \a url to the stream \a out and returns a reference
    to the stream.

    \sa \link datastreamformat.html Format of the QDataStream operators \endlink
*/
QDataStream &operator<<(QDataStream &out, const QUrl &url)
{
    QByteArray u = url.toString(QUrl::FullyEncoded).toLatin1();
    out << u;
    return out;
}

/*! \relates QUrl

    Reads a url into \a url from the stream \a in and returns a
    reference to the stream.

    \sa \link datastreamformat.html Format of the QDataStream operators \endlink
*/
QDataStream &operator>>(QDataStream &in, QUrl &url)
{
    QByteArray u;
    in >> u;
    url.setUrl(QString::fromLatin1(u));
    return in;
}
#endif // QT_NO_DATASTREAM

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug d, const QUrl &url)
{
    d.maybeSpace() << "QUrl(" << url.toDisplayString() << ')';
    return d.space();
}
#endif

/*!
    \since 4.2

    Returns a text string that explains why an URL is invalid in the case being;
    otherwise returns an empty string.
*/
QString QUrl::errorString() const
{
    if (!d)
        return QString();

    if (d->sectionHasError == 0)
        return QString();

    // check if the error code matches a section with error
    if ((d->sectionHasError & (d->errorCode >> 8)) == 0)
        return QString();

    QChar c = d->errorSupplement;
    switch (QUrlPrivate::ErrorCode(d->errorCode)) {
    case QUrlPrivate::NoError:
        return QString();

    case QUrlPrivate::InvalidSchemeError: {
        QString msg = QStringLiteral("Invalid scheme (character '%1' not permitted)");
        return msg.arg(c);
    }
    case QUrlPrivate::SchemeEmptyError:
        return QStringLiteral("Empty scheme");

    case QUrlPrivate::InvalidUserNameError:
        return QString(QStringLiteral("Invalid user name (character '%1' not permitted)"))
                .arg(c);

    case QUrlPrivate::InvalidPasswordError:
        return QString(QStringLiteral("Invalid password (character '%1' not permitted)"))
                .arg(c);

    case QUrlPrivate::InvalidRegNameError:
        if (d->errorSupplement)
            return QString(QStringLiteral("Invalid hostname (character '%1' not permitted)"))
                    .arg(c);
        else
            return QStringLiteral("Hostname contains invalid characters");
    case QUrlPrivate::InvalidIPv4AddressError:
        return QString(); // doesn't happen yet
    case QUrlPrivate::InvalidIPv6AddressError:
        return QStringLiteral("Invalid IPv6 address");
    case QUrlPrivate::InvalidIPvFutureError:
        return QStringLiteral("Invalid IPvFuture address");
    case QUrlPrivate::HostMissingEndBracket:
        return QStringLiteral("Expected ']' to match '[' in hostname");

    case QUrlPrivate::InvalidPortError:
    case QUrlPrivate::PortEmptyError:
        return QStringLiteral("Invalid port or port number out of range");

    case QUrlPrivate::InvalidPathError:
        return QString(QStringLiteral("Invalid path (character '%1' not permitted)"))
                .arg(c);
    case QUrlPrivate::PathContainsColonBeforeSlash:
        return QStringLiteral("Path component contains ':' before any '/'");

    case QUrlPrivate::InvalidQueryError:
        return QString(QStringLiteral("Invalid query (character '%1' not permitted)"))
                .arg(c);

    case QUrlPrivate::InvalidFragmentError:
        return QString(QStringLiteral("Invalid fragment (character '%1' not permitted)"))
                .arg(c);
    }
    return QStringLiteral("<unknown error>");
}

/*!
    \typedef QUrl::DataPtr
    \internal
*/

/*!
    \fn DataPtr &QUrl::data_ptr()
    \internal
*/

/*! \fn uint qHash(const QUrl &url)
    \relates QHash

    Returns the hash value for the \a url.
*/
uint qHash(const QUrl &url)
{
    if (!url.d)
        return qHash(-1); // the hash of an unset port (-1)

    return qHash(url.d->scheme) ^
            qHash(url.d->userName) ^
            qHash(url.d->password) ^
            qHash(url.d->host) ^
            qHash(url.d->port) ^
            qHash(url.d->path) ^
            qHash(url.d->query) ^
            qHash(url.d->fragment);
}


// The following code has the following copyright:
/*
   Copyright (C) Research In Motion Limited 2009. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of Research In Motion Limited nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY Research In Motion Limited ''AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL Research In Motion Limited BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/


/*!
    Returns a valid URL from a user supplied \a userInput string if one can be
    deducted. In the case that is not possible, an invalid QUrl() is returned.

    \since 4.6

    Most applications that can browse the web, allow the user to input a URL
    in the form of a plain string. This string can be manually typed into
    a location bar, obtained from the clipboard, or passed in via command
    line arguments.

    When the string is not already a valid URL, a best guess is performed,
    making various web related assumptions.

    In the case the string corresponds to a valid file path on the system,
    a file:// URL is constructed, using QUrl::fromLocalFile().

    If that is not the case, an attempt is made to turn the string into a
    http:// or ftp:// URL. The latter in the case the string starts with
    'ftp'. The result is then passed through QUrl's tolerant parser, and
    in the case or success, a valid QUrl is returned, or else a QUrl().

    \section1 Examples:

    \list
    \li qt.nokia.com becomes http://qt.nokia.com
    \li ftp.qt.nokia.com becomes ftp://ftp.qt.nokia.com
    \li hostname becomes http://hostname
    \li /home/user/test.html becomes file:///home/user/test.html
    \endlist
*/
QUrl QUrl::fromUserInput(const QString &userInput)
{
    QString trimmedString = userInput.trimmed();

    // Check first for files, since on Windows drive letters can be interpretted as schemes
    if (QDir::isAbsolutePath(trimmedString))
        return QUrl::fromLocalFile(trimmedString);

    QUrl url = QUrl(trimmedString, QUrl::TolerantMode);
    QUrl urlPrepended = QUrl(QStringLiteral("http://") + trimmedString, QUrl::TolerantMode);

    // Check the most common case of a valid url with scheme and host
    // We check if the port would be valid by adding the scheme to handle the case host:port
    // where the host would be interpretted as the scheme
    if (url.isValid()
        && !url.scheme().isEmpty()
        && (!url.host().isEmpty() || !url.path().isEmpty())
        && urlPrepended.port() == -1)
        return url;

    // Else, try the prepended one and adjust the scheme from the host name
    if (urlPrepended.isValid() && (!urlPrepended.host().isEmpty() || !urlPrepended.path().isEmpty()))
    {
        int dotIndex = trimmedString.indexOf(QLatin1Char('.'));
        const QString hostscheme = trimmedString.left(dotIndex).toLower();
        if (hostscheme == ftpScheme())
            urlPrepended.setScheme(ftpScheme());
        return urlPrepended;
    }

    return QUrl();
}
// end of BSD code

QT_END_NAMESPACE
