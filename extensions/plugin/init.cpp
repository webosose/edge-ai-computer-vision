#include <aif/plugin/Plugin.h>

extern "C" {

aif::Plugin* plugin_init()
{
  aif::Plugin *plugin = new aif::Plugin();
  plugin->setAlias("Base");
  plugin->setVersion("1.0.0");
  plugin->setDescription("Base plugin for Edge AI Vision");
  // plugin->makeHidden("face_yunet_360_640");
  // plugin->makeHidden("signlanguagearea_model_cpu");
  return plugin;
}

} // extern "C"