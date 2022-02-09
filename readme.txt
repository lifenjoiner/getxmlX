getxmlX

A simple xml querier for Windows.

Usage: getxmlX <operations> <xml-file | URL>
operation examples:
    selectNodes(data).length
    selectSingleNode(data).getAttribute(href)
    selectSingleNode(data).text
    selectSingleNode(data).childNodes.length
    selectSingleNode(b:book[@id='bk102'])
    selectSingleNode(string[@name='permlab_callPhone']).text
    selectNodes(permission[contains(@android:name,'CONTACTS')])
    selectNodes(string[starts-with(@name,'permlab_')])
    DOM
    [DOM.]childNodes
    getElementsByTagName(data).item(0).getAttribute(href)
    SelectionNamespaces("xmlns:a='http://myserver.com' xmlns:b='http://yourserver.com'").selectNodes(a:root/b:branch)
Tips:.
   Outputs OEM ANSI.
   All top namespaces selected.
   XPath functions: contains, starts-with.

Home
https://github.com/lifenjoiner/getxmlX
