/* COPYRIGHT (C) 2011-2013 ASSAF YARIV */
using System.Data.Entity;

namespace BServer.Models
{

    public class BServerContext : DbContext
    {
        // You can add custom code to this file. Changes will not be overwritten.
        // 
        // If you want Entity Framework to drop and regenerate your database
        // automatically whenever you change your model schema, add the following
        // code to the Application_Start method in your Global.asax file.
        // Note: this will destroy and re-create your database with every model change.
        // 
        // System.Data.Entity.Database.SetInitializer(new System.Data.Entity.DropCreateDatabaseIfModelChanges<BServer.Models.BServerContext>());

        public BServerContext() : base("name=BServerContext")
        {
        }

        public DbSet<Products> Products { get; set; }

        public DbSet<AVCfile> AVCfiles { get; set; }

        public DbSet<room> rooms { get; set; }
    }
}
