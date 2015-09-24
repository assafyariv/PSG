/* COPYRIGHT (C) 2011-2013 ASSAF YARIV */
using System.Web;
using System.Web.Optimization;

namespace BServer
{
    public class BundleConfig
    {
        public static void RegisterBundles(BundleCollection bundles)
        {
            bundles.Add(new ScriptBundle("~/bundles/jquery").Include(
                        "~/Scripts/jquery-1.*"));

            bundles.Add(new ScriptBundle("~/bundles/jqueryui").Include(
                        "~/Scripts/jquery-ui*"));

            bundles.Add(new ScriptBundle("~/bundles/jqueryval").Include(
                        "~/Scripts/jquery.unobtrusive*",
                        "~/Scripts/jquery.validate*"));

            bundles.Add(new ScriptBundle("~/bundles/modernizr").Include(
                        "~/Scripts/modernizr-*"));

            bundles.Add(new ScriptBundle("~/bundles/Three").Include(
                     "~/Scripts/three/build/Three.js"));


            bundles.Add(new ScriptBundle("~/bundles/ThreeJS").Include(
                        "~/Scripts/three/js/*.js",
                        "~/Scripts/three/js/loaders/*.js",
                        "~/Scripts/three/js/loaders/ctm/*.js",
                       "~/Scripts/three/js/postprocessing/*.js",
                       "~/Scripts/three/js/effects/*.js"
                       ));

            bundles.Add(new ScriptBundle("~/bundles/Text").Include(
                        "~/Scripts/helvetiker_bold.js"));

            bundles.Add(new ScriptBundle("~/bundles/Threex").Include(
               //  "~/Scripts/threex/examples/threex.domevent/vendor/three.js/*.js",
                 "~/Scripts/threex/examples/threex.domevent/vendor/three.js/RequestAnimationFrame.js",
                  "~/Scripts/threex/examples/threex.domevent/vendor/threex/*.js",
                  "~/Scripts/threex/examples/threex.domevent/vendor/tween.js/Tween.js",
                  "~/Scripts/threex/examples/threex.domevent/vendor/threex.dragpancontrols.js",
                  "~/Scripts/threex/examples/threex.domevent/vendor/threex.dragpancontrols2.js",
                  "~/Scripts/threex/threex.domevent.js",
                     "~/Scripts/threex/threex.domevent.object3d.js"
                //   "~/Scripts/threex.domevent.js"
                
                 ));

             bundles.Add(new StyleBundle("~/Content/catalog").Include(
                        "~/Content/themes/catalog.css"));

             bundles.Add(new StyleBundle("~/Content/pub").Include(
                         "~/Content/themes/pubList.css"));

            bundles.Add(new StyleBundle("~/Content/css").Include("~/Content/site.css"));

            bundles.Add(new StyleBundle("~/Content/themes/base/css").Include(
                        "~/Content/themes/base/jquery.ui.core.css",
                        "~/Content/themes/base/jquery.ui.resizable.css",
                        "~/Content/themes/base/jquery.ui.selectable.css",
                        "~/Content/themes/base/jquery.ui.accordion.css",
                        "~/Content/themes/base/jquery.ui.autocomplete.css",
                        "~/Content/themes/base/jquery.ui.button.css",
                        "~/Content/themes/base/jquery.ui.dialog.css",
                        "~/Content/themes/base/jquery.ui.slider.css",
                        "~/Content/themes/base/jquery.ui.tabs.css",
                        "~/Content/themes/base/jquery.ui.datepicker.css",
                        "~/Content/themes/base/jquery.ui.progressbar.css",
                        "~/Content/themes/base/jquery.ui.theme.css"));
        }
    }
}