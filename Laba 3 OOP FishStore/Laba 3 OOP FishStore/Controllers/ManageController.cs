using FishStore.Logic.DtoModels.Filters;
using Laba_3_OOP_FishStore.Features.DtoModels.Center;
using Laba_3_OOP_FishStore.Features.Interfaces.Managers;
using Microsoft.AspNetCore.Mvc;

namespace Laba_3_OOP_FishStore.Controllers
{
	public class ManageController : Controller
	{
		private readonly ICenterManager _centerManager;

		public ManageController(ICenterManager centerManager)
		{
			_centerManager = centerManager;
		}

		[HttpGet(nameof(Center), Name = nameof(Center))]
		public async Task<ActionResult> Center()
		{
			var center = _centerManager.GetListCenters(new CenterFilterDto()).FirstOrDefault();
			return View(center);
		}

		[HttpPost(nameof(CreateCenter), Name = nameof(CreateCenter))]
		public async Task<ActionResult> CreateCenter([FromBody] EditCenter center)
		{
			if (!ModelState.IsValid)
				return BadRequest(ModelState);

			_centerManager.Create(center);
			return Ok();
		}

		[HttpPost(nameof(CreateCenterView), Name = nameof(CreateCenterView))]
		public async Task<ActionResult> CreateCenterView([FromBody] EditCenter center)
		{
			if (!ModelState.IsValid)
				return View(nameof(Center), center);

			_centerManager.Create(center);
			return View();
		}

		[HttpPut(nameof(UpdateCenter), Name = nameof(UpdateCenter))]
		public async Task<ActionResult> UpdateCenter([FromBody] EditCenter center)
		{
			_centerManager.Update(center);
			return Ok();
		}

		[HttpPut(nameof(UpdateCenter), Name = nameof(UpdateCenter))]
		public async Task<ActionResult> DeleteCenter([FromQuery] Guid isnNode)
		{
			_centerManager.Delete(isnNode);
			return Ok();
		}

		[HttpGet(nameof(GetListCenters), Name = nameof(GetListCenters))]
		public async Task<ActionResult> GetListCenters()
		{
			var centers = _centerManager.GetListCenters(new CenterFilterDto());
			return Ok(centers);
		}

	}
}
