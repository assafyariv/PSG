/* COPYRIGHT (C) 2011-2013 ASSAF YARIV */
using System.Web;
using System.Web.Mvc;

namespace BServer
{
    public class FilterConfig
    {
        public static void RegisterGlobalFilters(GlobalFilterCollection filters)
        {
            filters.Add(new HandleErrorAttribute());
        }
    }
}