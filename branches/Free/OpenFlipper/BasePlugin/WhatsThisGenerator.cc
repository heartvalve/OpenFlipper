#include "WhatsThisGenerator.hh"

/** Constructor for the whatsThisGenerator initializing the plugin name.
 *
 * @param _plugin_name the name of the plugin which is used \note use only the name and NOT the prefix Plugin- (e.g. SkeletonEditing for Plugin-SkeletonEditing)
 */
WhatsThisGenerator::WhatsThisGenerator(const QString &_plugin_name) :
plugin_name_(_plugin_name)
{
}


/**	\brief generates a clickable link to the documentation for whatsThis Messages
 * @param _ref gives the section in the documentation for the action. The help-browser will jump into the section.
 *                      An example: _jump = "cube" in PrimitiveGenerator refs to the section "cube" in the Documentation and the help Browser.
 *
 * @param _site gives the documentation site (html-site) which you want to open.
 *                      Default is the site "index.html".
 *                      You need this parameter if you have more than one site in your plugin documentation and want to refer on one of this.
 */
QString WhatsThisGenerator::generateLink(const QString &_ref /*= ""*/, const QString &_site /*= "index.html"*/) const
{
  const QString baseHelpURL = QString(" <a href='qthelp://org.openflipper.plugin-")
                                         + plugin_name_.toLower()
                                         + QString("/Plugin-")
                                         + plugin_name_+QString("/")
                                         + _site+QString("#")+_ref + QString("'>");

  const QString clickText = QString(QObject::tr("Click for more information"))+QObject::tr("</a>");
  return (baseHelpURL + clickText);
}

/**	\brief sets a whatsThis Message plus link to the doc for the given QAction
 * @param _action action where the message should appear.
 * @param _msg help message for the user. This is the default Whatsthis Text which you set with QAction::setWhatsThis
 *
 * @param _ref gives the section in the documentation for the action. The help-browser will jump into the section.
 * 	                    An example: _jump = "cube" in PrimitiveGenerator refs to the section "cube" in the Documentation and the help Browser.
 *
 * @param _site gives the documentation site (html-site) which you want to open.
 * 	                    Default is the site "index.html".
 * 	                    You need this parameter if you have more than one site in your plugin documentation and want to refer on one of this.
 */
void WhatsThisGenerator::setWhatsThis(QAction* _action, const QString &_msg, const QString &_ref /*= ""*/,const QString &_site /*= "index.html"*/) const
{
  _action->setWhatsThis(_msg + generateLink(_ref, _site));
}
