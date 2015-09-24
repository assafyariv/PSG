/* COPYRIGHT (C) 2011-2013 ASSAF YARIV */
using System;
using System.Collections.Generic;
using System.Data;
using System.Data.Entity;
using System.Linq;
using System.Web;
using System.Web.Mvc;
using BServer.Models;
using System.IO;

namespace BServer.Controllers
{
    public class AVcfileController : Controller
    {
        private BServerContext db = new BServerContext();

        //
        // GET: /AVcfile/

        public ActionResult Index()
        {
            return View(db.AVCfiles.ToList());
        }

        //
        // GET: /AVcfile/Details/5

        public ActionResult Details(int id = 0)
        {
            AVCfile avcfile = db.AVCfiles.Find(id);
            if (avcfile == null)
            {
                return HttpNotFound();
            }
            return View(avcfile);
        }

        //
        // GET: /AVcfile/Create
         [Authorize]
        public ActionResult Create(int Pid = 0)
        {
            AVCfile av = new AVCfile();
            av.productId = Pid;
           // ViewBag.PRoID = Pid;
            ViewBag.ro = db.rooms.ToList();
            return View(av);
        }

        //
        // POST: /AVcfile/Create

        [HttpPost]
        public ActionResult Create(AVCfile avcfile, HttpPostedFileBase file)
        {
            if (ModelState.IsValid && file != null)
            {
                if (file.ContentLength > 0)
                {
              
                   Products product = db.Products.Find(avcfile.productId);
                   var dir = Path.Combine(Server.MapPath("~/"), product.location);

                    int htmlNum =0;
                    avcfile.version = 1;
                    avcfile.location = product.location + "/" + ProductsController.saveAndExtracr(file, dir, ref htmlNum, avcfile.version);
                  //  avcfile.product = product;
                  //  product.Publications.Add(avcfile);

                    db.AVCfiles.Add(avcfile);
                    avcfile.htmlNum = htmlNum;
                    avcfile.date = DateTime.Now;
                    avcfile.product = product;
                    avcfile.puroom = db.rooms.Find(avcfile.roomID);
                    product.Publications.Add(avcfile);
                    db.SaveChanges();
                    return RedirectToAction("FullDetails", "Products", new { id = avcfile.productId });
                }
            }

            return View(avcfile);
        }

        //
        // GET: /AVcfile/Edit/5
         [Authorize]
        public ActionResult Edit(int id = 0)
        {
            AVCfile avcfile = db.AVCfiles.Find(id);
            ViewBag.ro = db.rooms.ToList();
            if (avcfile == null)
            {
                return HttpNotFound();
            }
            return View(avcfile);
        }

        //
        // POST: /AVcfile/Edit/5
        // TBD - Assaf.
     /*   [HttpPost]
        [Authorize]
        public ActionResult Edit(AVCfile avcfile)
        {
            if (ModelState.IsValid)
            {
                db.Entry(avcfile).State = EntityState.Modified;
                db.SaveChanges();
                return RedirectToAction("FullDetails", "Products", new { id = avcfile.productId });
            }
            return View(avcfile);
        }
      */ 

        [HttpPost]
        [Authorize]
        public ActionResult Edit(AVCfile avcfile, HttpPostedFileBase file)
        {
            if (ModelState.IsValid)
            {
                AVCfile p = db.AVCfiles.Find(avcfile.id);

                if (file != null && file.ContentLength > 0)
                {
                    Products product = db.Products.Find(avcfile.productId);
                    var dir = Path.Combine(Server.MapPath("~/"), product.location);

                    int htmlNum = 0;
                    avcfile.version += 1;
                    avcfile.location = product.location + "/" + ProductsController.saveAndExtracr(file, dir, ref htmlNum, avcfile.version);
                    avcfile.htmlNum = htmlNum;

                    p.version = avcfile.version;
                    p.htmlNum = avcfile.htmlNum;
                    p.location = avcfile.location;
                }

                avcfile.date = DateTime.Now;
                if (p.name != avcfile.name)
                    p.name = avcfile.name;

                if (p.productVersion != avcfile.productVersion)
                    p.productVersion = avcfile.productVersion;

                if (p.description != avcfile.description)
                    p.description = avcfile.description;

                if (p.roomID != avcfile.roomID)
                {
                    p.roomID = avcfile.roomID;
                    p.puroom = db.rooms.Find(avcfile.roomID);
                }

                //db.Entry(products).State = EntityState.Modified;
                db.SaveChanges();
                return RedirectToAction("Plist", "Products");
            }
            //print erroe
            foreach (var obj in ModelState.Values)
            {
                foreach (var error in obj.Errors)
                {
                    if (!string.IsNullOrEmpty(error.ErrorMessage))
                        System.Diagnostics.Debug.WriteLine("ERROR WHY = " + error.ErrorMessage);
                    System.Diagnostics.Debug.WriteLine(error.Exception);
                }
            }
          
            //ModelState.AddModelError("name", "nnnn");
            ViewBag.ro = (db.rooms.ToList());
            return View(avcfile);
        }



        public ActionResult model(int id = 0)
        {
            AVCfile avcfile = db.AVCfiles.Find(id);
            if (avcfile == null)
            {
                return View();
            }
            return View(avcfile);
        }

        public ActionResult FullSModelP(int id = 0)
        {
            AVCfile avcfile = db.AVCfiles.Find(id);
            if (avcfile == null)
            {
                return View();
            }
            return View(avcfile);
        }

        public ActionResult PubDetalis(int id = 0)
        {
            AVCfile avcfile = db.AVCfiles.Find(id);
            if (avcfile == null)
            {
                return View();
            }
            return View(avcfile);
        }

        

        //
        // GET: /AVcfile/Delete/5
         [Authorize]
        public ActionResult Delete(int id = 0)
        {
            AVCfile avcfile = db.AVCfiles.Find(id);
            if (avcfile == null)
            {
                return HttpNotFound();
            }
            return View(avcfile);
        }

        //
        // POST: /AVcfile/Delete/5
         [Authorize]
        [HttpPost, ActionName("Delete")]
        public ActionResult DeleteConfirmed(int id)
        {
            AVCfile avcfile = db.AVCfiles.Find(id);
            db.AVCfiles.Remove(avcfile);
            db.SaveChanges();
            return RedirectToAction("FullDetails", "Products", new { id = avcfile.productId });
        }



         public FileResult Download(int id = 0)
         {
             Products p = db.Products.Find(id);
             if (p != null)
             {
                 var dir = Path.Combine(Server.MapPath("~/"), p.location);
                 string contentType = "application/zip";
                 // TODO : Fix this crap below - assaf.
                 //Parameters to file are
                 //1. The File Path on the File Server
                 //2. The content type MIME type
                 //3. The parameter for the file save by the browser
                 
                 return File(dir + ".avc", contentType, "model.avc");
             }
             return null;
         }

        protected override void Dispose(bool disposing)
        {
            db.Dispose();
            base.Dispose(disposing);
        }
    }
}