using Microsoft.AspNetCore.Mvc;

namespace Laba_3_OOP_FishStore.Controllers
{
    [Route("")]
    public class HomeController : Controller
    {
        public HomeController()
        {

        }
        [HttpGet, Route("")]
        public ActionResult Index()
        {
            return View();
        }
    }
}
