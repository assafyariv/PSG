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
    public class RoomController : Controller
    {
        private BServerContext db = new BServerContext();

        //
        // GET: /Room/
         [Authorize]
        public ActionResult Index()
        {
            return View(db.rooms.ToList());
        }

        //
        // GET: /Room/Details/5
         [Authorize]
        public ActionResult Details(int id = 0)
        {
            room room = db.rooms.Find(id);
            if (room == null)
            {
                return HttpNotFound();
            }
            return View(room);
        }

        //
        // GET: /Room/Create
         [Authorize]
        public ActionResult Create()
        {
             
            return View();
        }

        //
        // POST: /Room/Create

        [HttpPost]
        [Authorize]
        public ActionResult Create(room room, HttpPostedFileBase file)
        {
            if (ModelState.IsValid && file != null)
            {
                if (file.ContentLength > 0)
                {
                    var dir = Path.Combine(Server.MapPath("~/Content/Romms"), room.name);
                    Directory.CreateDirectory(dir);

                    var fileName = Path.GetFileName(file.FileName);
                    var path = Path.Combine(dir, fileName);
                    file.SaveAs(path);

                    string unpackDirectory = Path.GetFileNameWithoutExtension(file.FileName);
                    var pathUnpackDirectory = Path.Combine(dir, unpackDirectory);
                    Directory.CreateDirectory(pathUnpackDirectory);

                    using (ZipFile zip1 = ZipFile.Read(path))
                    {
                        // here, we extract every entry, but we could extract conditionally
                        // based on entry name, size, date, checkbox status, etc. 
                        // TODO: Fix this crap later -assaf
                        foreach (ZipEntry e in zip1)
                        {
                            e.Extract(pathUnpackDirectory, ExtractExistingFileAction.OverwriteSilently);
                        }
                    }

                    room.location = "/BServer/Content/Romms/" + room.name + "/" + unpackDirectory;
                    db.rooms.Add(room);
                    db.SaveChanges();
                    return RedirectToAction("Index");
                }
            }
            return View(room);
        }

        //
        // GET: /Room/Edit/5
         [Authorize]
        public ActionResult Edit(int id = 0)
        {
            room room = db.rooms.Find(id);
            if (room == null)
            {
                return HttpNotFound();
            }
            return View(room);
        }

        //
        // POST: /Room/Edit/5
         [Authorize]
        [HttpPost]
        public ActionResult Edit(room room)
        {
            if (ModelState.IsValid)
            {
                
                db.Entry(room).State = EntityState.Modified;
                db.SaveChanges();
                return RedirectToAction("Index");
            }
            return View(room);
        }

        //
        // GET: /Room/Delete/5
         [Authorize]
        public ActionResult Delete(int id = 0)
        {
            room room = db.rooms.Find(id);
            if (room == null)
            {
                return HttpNotFound();
            }
            return View(room);
        }

        //
        // POST: /Room/Delete/5
         [Authorize]
        [HttpPost, ActionName("Delete")]
        public ActionResult DeleteConfirmed(int id)
        {
            room room = db.rooms.Find(id);
            db.rooms.Remove(room);
            db.SaveChanges();
            return RedirectToAction("Index");
        }

        protected override void Dispose(bool disposing)
        {
            db.Dispose();
            base.Dispose(disposing);
        }
    }
}