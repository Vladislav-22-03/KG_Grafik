using Laba_3_OOP_FishStore.Features.DtoModels.User;
using Laba_3_OOP_FishStore.Features.Interfaces.Managers;
using Microsoft.AspNetCore.Mvc;

namespace Laba_3_OOP_FishStore.Controllers
{
    [Route("Registration")]
    public class RegistrationController : Controller
    {
        private readonly IUserManager userManager;

        public RegistrationController(IUserManager userManager)
        {
            this.userManager = userManager;
        }


        [HttpGet, Route("Register")]
        public async Task<ActionResult> Register()
        {
            return View();
        }

        [HttpPost(nameof(CreateRegister), Name = "CreateRegister")]
        public async Task<ActionResult> CreateRegister(EditUser model)
        {
            if (ModelState.IsValid)
            {

                // Добавление пользователя в контекст данных и сохранение его в базе данных
                userManager.Create(model);

                // Пример редиректа на главную страницу после регистрации
                return RedirectToAction("Index", "Home");
            }

            // Если валидация модели не удалась, вернуть представление с ошибками
            return View(nameof(Register), model);
        }


        //[HttpGet, Route("Login")]
        //public async Task<ActionResult> Login()
        //{
        //	return View();
        //}

        //[HttpPost(nameof(CreateLogin), Name = "CreateLogin")]
        //public async Task<ActionResult> CreateLogin(User USER)
        //{
        //	User user = userManager.FindByFirstName(USER.FirstName);

        //	// Если пользователь найден, аутентифицировать его и перенаправить на главную страницу
        //	if (user != null)
        //	{
        //		// Реализуйте аутентификацию пользователя
        //		// Например, можно использовать HttpContext.SignInAsync() для аутентификации пользователя

        //		// Перенаправить на главную страницу
        //		return RedirectToAction("Index", "Main", new { id = user.IsnNode });
        //	}

        //	// Если пользователь не найден, вернуть представление с сообщением об ошибке
        //	ModelState.AddModelError(string.Empty, "Invalid login attempt.");
        //	return View();
        //}


    }
}
