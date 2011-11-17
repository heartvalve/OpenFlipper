from lxml import etree
import StringIO
import sys

TAGfile = open("./Testing/TAG", 'r')
dirname = TAGfile.readline().strip()

xmlfile = open("./Testing/"+dirname+"/Test.xml", 'r')
xslfile = open("CTest2JUnit.xsl", 'r')

xmlcontent = xmlfile.read()
xslcontent = xslfile.read()

xmldoc = etree.parse(StringIO.StringIO(xmlcontent))
xslt_root = etree.XML(xslcontent)
transform = etree.XSLT(xslt_root)

result_tree = transform(xmldoc)
print(result_tree)
