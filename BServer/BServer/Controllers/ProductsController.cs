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
using Ionic.Zip;


namespace BServer.Controllers
{
    public class ProductsController : Controller
    {
        private BServerContext db = new BServerContext();

        //
        // GET: /Products/

        public ActionResult Index()
        {
           // if (Request.IsAuthenticated)
             //   RedirectToAction("Plist");
            

            return View(db.Products.ToList());
        }

        //
        // GET: /Products/Details/5
        
        public ActionResult Details(int id = 0)
        {
            Products products = db.Products.Find(id);
            if (products == null)
            {
                return HttpNotFound();
            }
            return View(products);
        }

        [Authorize]//(Users = "assaf", Roles = "Admin, SuperUser")]
        public ActionResult Plist()
        {
            return View(db.Products.ToList());
        }
  
        //
        // GET: /Products/Create

        public ActionResult Create()
        {
            //var query = db.rooms.Select(c => new { c.roomId, c.name });
            //ViewBag.ro = new SelectList(query.AsEnumerable(), "roomId", "name", 3);
            ViewBag.ro =  (db.rooms.ToList());
            return View();
        }

        public static void doPythonScript(string path)
        {
            var p = @"C:\Education\BServer\BServer\BServer\Content\convert_obj_three.py";
            string command = " \"" + p + "\" -i \"" + path + ".obj\" -o \"" + path + ".js\" -a center";

          
                System.Diagnostics.Process prcPythonCheck = new System.Diagnostics.Process();
                prcPythonCheck.StartInfo.UseShellExecute = false;
                prcPythonCheck.StartInfo.RedirectStandardOutput = true;
                prcPythonCheck.StartInfo.CreateNoWindow = true;
                prcPythonCheck.StartInfo.FileName = "python";
                prcPythonCheck.StartInfo.Arguments = command;

                prcPythonCheck.Start();
                string result = prcPythonCheck.StandardOutput.ReadToEnd();
                prcPythonCheck.WaitForExit();
           
             
        }
        //
        // POST: /Products/Create

        public static string saveAndExtracr(HttpPostedFileBase file, string dir, ref int htmlNum, int version)
        {
            var fileName = Path.GetFileName(file.FileName); //with extension
            string unpackDirectory = Path.GetFileNameWithoutExtension(file.FileName);
            var pathUnpackDirectory = Path.Combine(dir, unpackDirectory + Convert.ToString(version));

            var path = pathUnpackDirectory + ".avc";//Path.Combine(dir, fileName);
            file.SaveAs(path);
            Directory.CreateDirectory(pathUnpackDirectory);

            using (ZipFile zip1 = ZipFile.Read(path))
            {
                // here, we extract every entry, but we could extract conditionally
                // based on entry name, size, date, checkbox status, etc.  
                foreach (ZipEntry e in zip1)
                {
                    e.Extract(pathUnpackDirectory, ExtractExistingFileAction.OverwriteSilently);
                }
            }

                  var count=0;
                string [] fileEntries = Directory.GetFiles(pathUnpackDirectory);
                foreach(string f in fileEntries)
                {
                   var extension = Path.GetExtension(f);
                   if (extension.Equals(".htm"))
                    {  
                        count ++; 
                    }
                   else if (extension.Equals(".obj"))
                    {
                       string modelPath =  Path.Combine(pathUnpackDirectory, Path.GetFileNameWithoutExtension(f));
                       try
                       {
                           doPythonScript(modelPath);
                       }

                       catch (Exception objException)
                       {
                           Console.WriteLine(objException.Message);
                           return null;
                           // Log the exception and errors.
                       }
                    }
                }
             htmlNum = count;

            return unpackDirectory+version;
        }

        public ActionResult CreatePUB(int id = 0)
        {
            return RedirectToAction("Create", "AVcfile", new { Pid = id });
        }
        //
        // POST: /Product/Create

        [HttpPost]
        public ActionResult Create(Products product, HttpPostedFileBase file)
        {
            if (ModelState.IsValid && file != null)
            {
                if (file.ContentLength > 0)
                {
                    string name = product.productName.Replace(" ", "_");
                    var dir = Path.Combine(Server.MapPath("~/Content"), name);
                    if(!Directory.Exists(dir))
                    {
                    Directory.CreateDirectory(dir);
                    product.version = 1;
                    int htmlNum = 0 ;
                    product.location = "Content/" + name;
                    product.mainPubLocation = product.location + "/" + saveAndExtracr(file, dir, ref htmlNum, product.version);

                    product.htmlNum = htmlNum;
                    product.date = DateTime.Now;
                   
                    product.proom = db.rooms.Find(product.roomID);
                    db.Products.Add(product);
                    db.SaveChanges();
                    return RedirectToAction("Plist");
                    }
                   // ViewBag.fileError = "file with the ame name is rdy exists for this produrct";
                }
            }
            ViewBag.ro = (db.rooms.ToList());
            return View(product);
        }

        //
        // GET: /Products/model/5

        public ActionResult model(int id = 0)
        {
            Products products = db.Products.Find(id);
            if (products == null)
            {
                return View();
            }
            return View(products);
        }

        //
        // GET: /Products/FullSModel/5
        public ActionResult FullSModel(int id = 0)
        {
            Products products = db.Products.Find(id);
            if (products == null)
            {
                return View();
            }
            return View(products);
        }

        public ActionResult ToolBar(int id = 0)
        {
            Products products = db.Products.Find(id);
            if (products == null)
            {
                return View();
            }
            return View(products);
        }

        //
        // GET: /Products/FullSModel/5
        public ActionResult FullDetails(int id = 0)
        {
            Products products = db.Products.Find(id);
            if (products == null)
            {
                return View();
            }
            return View(products);
        }

        public ActionResult ViewHtml(int id = 0)
        {
            Products products = db.Products.Find(id);
            if (products == null)
            {
                return View();
            }
            return View(products);
        }


        
        //
        // GET: /Products/Edit/5
        [Authorize]
        public ActionResult Edit(int id = 0)
        {
            Products products = db.Products.Find(id);
            ViewBag.ro = db.rooms.ToList();
            if (products == null)
            {
                return HttpNotFound();
            }
            return View(products);
        }

        //
        // POST: /Products/Edit/5

        [HttpPost]
        [Authorize]
        public ActionResult Edit(Products products,  HttpPostedFileBase file)
        {
            if (ModelState.IsValid)
            {
                Products p = db.Products.Find(products.productID);

               if (file != null &&  file.ContentLength > 0)
                {
                   string name = products.productName.Replace(" ", "_");
                   var dir = Path.Combine(Server.MapPath("~/Content"), name);
                   int htmlNum = 0 ;
                   products.version += 1;
                   products.mainPubLocation = products.location + "/" + saveAndExtracr(file, dir, ref htmlNum, products.version);
                   products.htmlNum = htmlNum;

                   p.version = products.version;
                   p.htmlNum = products.htmlNum;
                   p.mainPubLocation = products.mainPubLocation;
                }

               products.date = DateTime.Now;
               if (p.productName != products.productName)
                   p.productName = products.productName;

               if (p.productVersion != products.productVersion)
                   p.productVersion = products.productVersion;

               if (p.description != products.description)
                   p.description = products.description;

               if (p.roomID != products.roomID)
               {
                   p.roomID = products.roomID;
                   p.proom = db.rooms.Find(products.roomID);
               }

                //db.Entry(products).State = EntityState.Modified;
                db.SaveChanges();
                return RedirectToAction("Plist");
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
            return View(products);
        }

        public ActionResult addPublicationDelete(int id = 0)
        {
            Products products = db.Products.Find(id);
            if (products == null)
            {
                return HttpNotFound();
            }
            return RedirectToAction("Create", "AVCfiel", new { });
        }

        //
        // GET: /Products/Delete/5
        [Authorize]
        public ActionResult Delete(int id = 0)
        {
            Products products = db.Products.Find(id);
            if (products == null)
            {
                return HttpNotFound();
            }
            return View(products);
        }

        //
        // POST: /Products/Delete/5
        [Authorize]
        [HttpPost, ActionName("Delete")]
        public ActionResult DeleteConfirmed(int id)
        {
            Products products = db.Products.Find(id);
            
           // var dir = Path.Combine(Server.MapPath("~/"), products.location);
           // string[] fileEntries = Directory.GetFiles(dir);
           // foreach (string f in fileEntries)
            //{
             //   File.Delete(f);
           // }
            //Directory.Delete(dir);

            db.Products.Remove(products);
            db.SaveChanges();

            return RedirectToAction("Plist");
        }

         public FileResult Download(int id = 0)
        {
            Products p = db.Products.Find(id);
            if (p != null)
            {
                var dir = Path.Combine(Server.MapPath("~/"), p.mainPubLocation);
                string contentType = "application/zip";
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