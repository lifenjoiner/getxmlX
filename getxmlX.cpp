/* cl.exe /Os /MD getxmlX.cpp
typedef WCHAR OLECHAR;
typedef OLECHAR* BSTR;
typedef BSTR* LPBSTR;
https://msdn.microsoft.com/en-us/library/windows/desktop/ms221069(v=vs.85).aspx
VARIANT
https://msdn.microsoft.com/en-us/library/windows/desktop/ms221627(v=vs.85).aspx
//
gcc -Os -s getxmlX.cpp -luuid -lole32 -loleaut32 -lstdc++ -static
https://gcc.gnu.org/bugzilla/show_bug.cgi?id=50888
//
Why c++?
To use _variant_t (comutil.h).
*/


/*
oXMLDOMDocument.load(xmlSource);
//
xmldoc.setProperty("SelectionLanguage", "XPath");
//
oXMLDOMNode.selectNodes(expression);
oXMLDOMNodeList.item(index);
oXMLDOMNode.selectSingleNode(queryString);
//
oXMLDOMNode.nodeValue;
oXMLDOMNode.text;
//
oXMLDOMElement.getElementsByTagName(tagName);
//
get*
oXMLDOMElement.getAttribute(name);
oXMLDOMAttribute.value;
//
? objXMLDOMSelection.getProperty(name);
//
XPath:
https://msdn.microsoft.com/en-us/library/ms256115.aspx
*/


// MUST for COM
#ifndef UNICODE
#define UNICODE
#define _UNICODE
#endif

#include <tchar.h>
#include <stdlib.h>
#include <stdio.h>

#include <locale.h>
#include <mbctype.h>

#ifdef _MSC_VER

#import <msxml6.dll> raw_interfaces_only // portable to gcc
using namespace MSXML2;

#else

#include <comdef.h>
#include <initguid.h>
DEFINE_GUID(CLSID_DOMDocument60, 0x88D96A05, 0xF192, 0x11D4, 0xA6, 0x5F, 0x00, 0x40, 0x96, 0x32, 0x51, 0xE5);
DEFINE_GUID(IID_IXMLDOMDocument2, 0x2933BF95, 0x7B36, 0x11D2, 0xB2, 0x0E, 0x00, 0xC0, 0x4F, 0x98, 0x3E, 0x60);
#define __USE_MSXML2_NAMESPACE__
#include <msxml2.h>

#endif

#include "vector.h"

#ifdef _MSC_VER
MSXML2::
#endif
IXMLDOMElement  *DOM;

void PrintXmlNode(
#ifdef _MSC_VER
    MSXML2::
#endif
    IXMLDOMNode *NODE
) {
    BSTR xml;
    //
    if (!NODE) {return;}
    //
    NODE->get_xml(&xml);
    _tprintf(_T("%s\n"), xml);
}

void PrintXmlNodes(
#ifdef _MSC_VER
    MSXML2::
#endif
    IXMLDOMNodeList *NODES
) {
    long i, n;
#ifdef _MSC_VER
    MSXML2::
#endif
    IXMLDOMNode *NODE;
    //
    if (!NODES) {return;}
    //
    NODES->get_length(&n);
    for (i = 0; i < n; i++) {
        NODES->get_item(i, &NODE);
        PrintXmlNode(NODE);
    }
}

void PrintAttriVARIANT(VARIANT *va) {
    if (!va) {return;}
    if ((va->vt & VT_BSTR) == VT_BSTR || (va->vt & VT_LPWSTR) == VT_LPWSTR) {wprintf(L"%s\n", va->pbstrVal);}
    if ((va->vt & VT_LPSTR) == VT_LPSTR) {printf("%s\n", va->pcVal);}
}

void PrintBSTR(const BSTR s) {
    if (s == NULL) {return;}
    wprintf(L"%s\n", s);
}

HRESULT SelectionNamespaces(
    MSXML2::IXMLDOMDocument2 *pDOMDoc,
    BSTR ns
) {
    return pDOMDoc->setProperty((BSTR)L"SelectionNamespaces", (_variant_t)ns);
}

void SelectionAllNamespaces(
    MSXML2::IXMLDOMDocument2 *pDOMDoc,
#ifdef _MSC_VER
    MSXML2::
#endif
    IXMLDOMElement *dom
) {
    HRESULT hr;
#ifdef _MSC_VER
    MSXML2::
#endif
    IXMLDOMNamedNodeMap *pDomAttrs = NULL;
#ifdef _MSC_VER
    MSXML2::
#endif
    IXMLDOMNode *pA = NULL;
    BSTR xml;
    long i, l;
    //
    if (!dom) {return;}
    dom->get_attributes(&pDomAttrs);
    hr = pDomAttrs->get_length(&l);
    if (hr != S_OK) {return;}
    for (i = 0; i < l; i++) {
        hr = pDomAttrs->get_item(i, &pA);
        pA->get_xml(&xml);
        if (_wcsnicmp(xml, L"xmlns:", 6) == 0) {
            SelectionNamespaces(pDOMDoc, xml);
        }
    }
}

size_t _tcsichrn(_TCHAR *str, long i, _TCHAR c) {
    _TCHAR *p;
    size_t n;
    //
    n = 0;
    p = str - 1;
    if (i < 0) {i = _tcslen(str);}
    while ((p = _tcschr(p + 1, c)) != NULL && p - str < i) { n++; }
    return n;
}

/* ^_^  . */
void format_operations(_TCHAR* ops) {
    _TCHAR* p = ops - 1;
    //
    while (p = _tcschr(p + 1, _T('.')), p != NULL) {
        if (_istalpha(*(p+1))) {
            long i = p - ops;
            if (_tcsichrn(ops, i, _T('(')) == _tcsichrn(ops, i, _T(')')) &&
                _tcsichrn(ops, i, _T('[')) == _tcsichrn(ops, i, _T(']')) &&
                _tcsichrn(ops, i, _T('\'')) % 2 == 0 &&
                _tcsichrn(ops, i, _T('"')) % 2 == 0
            ) { *p = _T('\n'); }
        }
    }
}

/* () not in [] */
void format_opt(_TCHAR* opt) {
    _TCHAR* p = opt - 1;
    //
    while (p = _tcspbrk(p + 1, _T("()")), p != NULL) {
        long i, t;
        i = p - opt;
        t = _tcsichrn(opt, i, _T('['));
        if (t == 0 || t == _tcsichrn(opt, i, _T(']'))) { *p = _T('\n'); }
    }
}

void parse_operations_on_ws(
    MSXML2::IXMLDOMDocument2 *pDOMDoc,
#ifdef _MSC_VER
    MSXML2::
#endif
IXMLDOMNodeList *NODES,
#ifdef _MSC_VER
    MSXML2::
#endif
    IXMLDOMNode *NODE, const _TCHAR* ops
) {
    Vector operations, opr;
    _TCHAR *param;
    size_t i, n;
    //
    operations = _tcs_split(ops, _T("\n"));
    n = operations.length;
    //
    i = n;
    for (i = 0; i < n; i++) {
        _TCHAR *pp, *opti;
        int ii, nn;
        long NODES_l;
        //
        opti = _tcsdup((_TCHAR*)operations.items[i]);
        format_opt(opti);
        //
        opr = _tcs_split(opti, _T("\n"));
        free(opti);
        //
        nn = opr.length;
        pp = (_TCHAR*)opr.items[0];
        //
        if (!NODE) {NODE = DOM;}
        if (NODES) {NODES->get_length(&NODES_l);}
        //
        switch (nn) {
        case 3:
            goto EACH;
        case 2:
            param = (_TCHAR*)opr.items[1];
            if (_tcscmp(pp, _T("selectNodes")) == 0 || _tcscmp(pp, _T("getElementsByTagName")) == 0) {
                NODE->selectNodes(param, &NODES);
                NODE = NULL;
                if (i == n -1) { PrintXmlNodes(NODES); }
            }
            else if (_tcscmp(pp, _T("selectSingleNode")) == 0 && NODE) {
                NODE->selectSingleNode(param, &NODE);
                NODES = NULL;
                if (i == n -1) { PrintXmlNode(NODE); }
            }
            else if (_tcscmp(pp, _T("getAttribute")) == 0 && NODE) {
                VARIANT va;
#ifdef _MSC_VER
                MSXML2::
#endif
                IXMLDOMElement *pDOMElement = NULL;
                NODE->QueryInterface(
#ifdef _MSC_VER
                    __uuidof(MSXML2::IXMLDOMElement),
#else
                    IID_IXMLDOMElement,
#endif
                    (void**)&pDOMElement);
                pDOMElement->getAttribute(param, &va);
                PrintAttriVARIANT(&va);
                NODE = NULL;
            }
            else if (_tcscmp(pp, _T("item")) == 0 && NODES_l) {
                if (isdigit(*param)) {
                    ii = _ttoi(param);
                    NODES->get_item(ii, &NODE);
                    if (i == n -1) { PrintXmlNode(NODE); }
                }
                else { goto UNKNOWN; }
            }
            else if (_tcscmp(pp, _T("SelectionNamespaces")) == 0 && DOM) {
                SelectionNamespaces(pDOMDoc, param);
            }
            else
EACH:
            if (_tcscmp(pp, _T("each")) == 0 && NODES_l) {
                long j, l;
#ifdef _MSC_VER
                MSXML2::
#endif
                IXMLDOMNodeList *NODES2 = NULL;
#ifdef _MSC_VER
                MSXML2::
#endif
                IXMLDOMNode *NODE2 = NULL;
                //
                for (j = 0; j < NODES_l; j++) {
                    NODES->get_item(j, &NODE2);
                    parse_operations_on_ws(pDOMDoc, NODES2, NODE2, (_TCHAR*)(operations.items[i]) + 5);
                }
            }
            else { goto UNKNOWN; }
            break;
        case 1:
            if (_tcscmp(pp, _T("text")) == 0 && NODE) {
                BSTR s;
                NODE->get_text(&s);
                PrintBSTR(s);
            }
            else if (_tcscmp(pp, _T("length")) == 0 && NODES) {
                _tprintf(_T("%d\n"), NODES_l);
            }
            else if (_tcscmp(pp, _T("DOM")) == 0) {
                if (i == 0 && i == n - 1) {
                    PrintXmlNode(NODE);
                }
            }
            else if (_tcscmp(pp, _T("childNodes")) == 0 && NODE) {
                NODE->get_childNodes(&NODES);
                NODE = NULL;
                if (i == n -1) { PrintXmlNodes(NODES); }
            }
            else { goto UNKNOWN; }
            break;
        default:
UNKNOWN:
            _ftprintf(stderr, _T("Err: %s\n"), operations.items[i]);
            i = n;
            break;
        }
        vector_free(&opr);
    }
    vector_free(&operations);
}

/**/
void help(const _TCHAR *app) {
    _tprintf(_T("Get information from xml. msxml 6 based, XPath flavor. v0.1.1 by YX Hao\n"));
    _tprintf(_T("Usage: %s <operations> <xml-file | URL>\n"), app);
    _tprintf(_T("operation examples:\n"));
    _tprintf(_T("    selectNodes(data).length\n"));
    _tprintf(_T("    selectSingleNode(data).getAttribute(href)\n"));
    _tprintf(_T("    selectSingleNode(data).text\n"));
    _tprintf(_T("    selectSingleNode(data).childNodes.length\n"));
    _tprintf(_T("    selectSingleNode(b:book[@id='bk102'])\n"));
    _tprintf(_T("    selectSingleNode(string[@name='permlab_callPhone']).text\n"));
    _tprintf(_T("    selectNodes(permission[contains(@android:name,'CONTACTS')])\n"));
    _tprintf(_T("    selectNodes(string[starts-with(@name,'permlab_')])\n"));
    _tprintf(_T("    DOM\n"));
    _tprintf(_T("    [DOM.]childNodes\n"));
    _tprintf(_T("    getElementsByTagName(data).item(0).getAttribute(href)\n"));
    _tprintf(_T("    SelectionNamespaces(\"xmlns:a='http://myserver.com' xmlns:b='http://yourserver.com'\").selectNodes(a:root/b:branch)\n"));
    _tprintf(_T("Tips:.\n"));
    _tprintf(_T("   Outputs OEM ANSI.\n"));
    _tprintf(_T("   All top namespaces selected.\n"));
    _tprintf(_T("   XPath functions: contains, starts-with.\n"));
    exit(EXIT_FAILURE);
}

#ifdef _MSC_VER

int wmain(int argc, wchar_t *argv[]) {

#else

typedef struct
{
    int newmode;
} _startupinfo;

extern
#ifdef __cplusplus
"C"
#endif
int __cdecl __wgetmainargs(int *pargc, wchar_t ***pargv, wchar_t ***penv, int globb, _startupinfo*);

int main() {
    int argc;
    _TCHAR **argv, **env;
    __wgetmainargs(&argc, &argv, &env, 0, NULL);

#endif

    _TCHAR MBCP[8];
    //
    if (argc != 3) {
        help(argv[0]);
        return 87; //ERROR_INVALID_PARAMETER
    }
    // tricks
    _stprintf(MBCP, _T(".%d"), _getmbcp());
    _tsetlocale(LC_CTYPE, MBCP);
    //
    CoInitialize(NULL);
    //
    MSXML2::IXMLDOMDocument2    *pXMLDoc = NULL;
    VARIANT_BOOL        bLoad = FALSE;
    //
    HRESULT hr = CoCreateInstance(
#ifdef _MSC_VER
            __uuidof(MSXML2::DOMDocument60), 
#else
            CLSID_DOMDocument60,
#endif
            NULL, CLSCTX_INPROC_SERVER,
#ifdef _MSC_VER
            __uuidof(MSXML2::IXMLDOMDocument2),
#else
            IID_IXMLDOMDocument2,
#endif
            (void **)&pXMLDoc
    );
    //
    pXMLDoc->put_async(VARIANT_FALSE);
    //
    hr = pXMLDoc->load((_variant_t)argv[2], &bLoad);
    //
    if(hr != S_OK) {
#ifdef _MSC_VER
        MSXML2::
#endif
        IXMLDOMParseError   *pParseError;
        BSTR                        *errStr = NULL;
        pXMLDoc->get_parseError(&pParseError);
        _ftprintf(stderr, _T("Err: %d\n"), hr);
        pParseError->get_reason(errStr);
        _ftprintf(stderr, _T("Reason: %s\n"), errStr);
        return 1;
    }
    //
#ifdef _MSC_VER
    MSXML2::
#endif
    IXMLDOMNodeList *NODES = NULL;
#ifdef _MSC_VER
    MSXML2::
#endif
    IXMLDOMNode     *NODE = NULL;
    pXMLDoc->get_documentElement(&DOM);
    //
    pXMLDoc->setProperty((BSTR)L"SelectionLanguage", (_variant_t)L"XPath");
    pXMLDoc->setProperty((BSTR)L"AllowDocumentFunction", (_variant_t)VARIANT_TRUE);
    //
    SelectionAllNamespaces(pXMLDoc, DOM);
    //
    format_operations(argv[1]);
    //
    parse_operations_on_ws(pXMLDoc, NODES, NODE, argv[1]);
    //
    CoUninitialize();
    //
    return 0;
}
