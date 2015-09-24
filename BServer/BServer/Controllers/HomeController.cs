/* COPYRIGHT (C) 2011-2013 ASSAF YARIV */
using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.Mvc;
using System.IO;

namespace BServer.Controllers
{
    public class HomeController : Controller
    {
        public ActionResult Index()
        {
            ViewBag.Message = "Modify this template to kick-start your ASP.NET MVC application.";

            return View();
        }

        public ActionResult About()
        {
            ViewBag.Message = "Software components and applications for model visualization ";

            return View();
        }

        public ActionResult Contact()
        {
            ViewBag.Message = "Thank you for visiting our site !";


            return View();
        }

        public ActionResult home()
        {
            return View();
        }

        public ActionResult background()
        {
            return View();
        }


        [HttpPost]
        public ActionResult background(HttpPostedFileBase FileUpload1, HttpPostedFileBase FileUpload2,
            HttpPostedFileBase FileUpload3, HttpPostedFileBase FileUpload4)
        {
            var dir = Path.Combine(Server.MapPath("~/"), "Images");
            if (FileUpload1 != null && FileUpload1.ContentLength > 0)//TODO   && FileUpload1.ContentType == "Image/jpg")
            {
              var path = Path.Combine(dir, "header_background.jpg");
              FileUpload1.SaveAs(path);  
            }
            if (FileUpload2 != null && FileUpload2.ContentLength > 0)
            {
                var path = Path.Combine(dir, "body_background.jpg");
                FileUpload2.SaveAs(path); 
            }
            if (FileUpload3 != null && FileUpload3.ContentLength > 0)
            {
                var path = Path.Combine(dir, "footer_background.jpg");
                FileUpload3.SaveAs(path); 
            }
            if (FileUpload4 != null && FileUpload4.ContentLength > 0)
            {
                var path = Path.Combine(dir, "logo.jpg");
                FileUpload3.SaveAs(path); 
            }
            return Redirect(Request.UrlReferrer.ToString());
           // return RedirectToAction("Plist", "Products"); //TODO check for error!!!
            //return View();
        }

         [HttpPost]
        public ActionResult backgroundDef()
        {
            var dir = Path.Combine(Server.MapPath("~/"), "Images");

            var path = Path.Combine(dir, "header_background");
            System.IO.File.Copy(path + "_ori.jpg", path + ".jpg", true);

             path = Path.Combine(dir, "body_background");
            System.IO.File.Copy(path + "_ori.jpg", path + ".jpg" , true);

             path = Path.Combine(dir, "footer_background");
            System.IO.File.Copy(path + "_ori.jpg", path + ".jpg", true);

             path = Path.Combine(dir, "logo");
            System.IO.File.Copy(path + "_ori.jpg", path + ".jpg", true);
              
            return Redirect(Request.UrlReferrer.ToString());
            //return RedirectToAction("Plist", "Products");
        }
    }
}
