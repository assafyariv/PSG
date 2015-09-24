/* COPYRIGHT (C) 2011-2013 ASSAF YARIV */
using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.ComponentModel.DataAnnotations;
using System.Data.Entity;
using System.ComponentModel.DataAnnotations.Schema;

namespace BServer.Models
{

    public class room
    {
        [Key]
        public int roomId { get; set; }
        public string location { get; set; }
        public string name { get; set; }
        public string typ { get; set; }
        public int scale { get; set; }

    }

    public class Products
    {
        [Key]
        public int productID { get; set; }

        [Required]
        [MaxLength(100)]//, ErrorMessage = "The {0} must be at least {2} characters lolong.", MinimumLength = 6)]
        [Display(Name = "Name")]
        public string productName { get; set; }

        public string location { get; set; }
        public string mainPubLocation { get; set; }

        public int version { get; set; }

        [Display(Name = "Ver")]
        public string productVersion { get; set; }


        public int htmlNum { get; set; }

        [StringLength(1000, ErrorMessage = "The {0} must be at least {2} characters long.", MinimumLength = 6)]
        [Display(Name = "Description")]
        public string description { get; set; }

        [DataType(DataType.DateTime)]
        public DateTime? date { get; set; }

         [Display(Name = "Room")]
        public int roomID { get; set; }
        [ForeignKey("roomID")]
        public virtual room proom{ get; set; }

        public virtual ICollection<AVCfile> Publications { get; set; }
        public virtual ICollection<AVCfile> Materials { get; set; }
    }


    public class AVCfile
    {
        [Key]
        public int id { get; set; }

        [Required]
        [MaxLength(100)]
        [Display(Name = "Catalog name")]
        public string name { get; set; }

        public string location { get; set; }

        public int version { get; set; }

        [Display(Name = "Ver")]
        public string productVersion { get; set; }

        public int htmlNum { get; set; }

        [StringLength(1000, ErrorMessage = "The {0} must be at least {2} characters long.", MinimumLength = 6)]
        [Display(Name = "Description")]
        public string description { get; set; }

         [DataType(DataType.DateTime)]
        public DateTime? date { get; set; }

         public string data { get; set; }

         [Display(Name = "Room")]
         public int roomID { get; set; }
         public virtual room puroom { get; set; }

         public int productId { get; set; }
        public virtual Products product { get; set; }


    }

}
