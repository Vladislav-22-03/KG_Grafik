using AutoMapper;
using FishStore.Logic.Interfaces.Repositories;
using FishStore.Logic.Interfaces.Services;
using FishStore.Storage.Database;
using FishStore.Storage.Models;
using Laba_3_OOP_FishStore.Features.DtoModels.User;
using Laba_3_OOP_FishStore.Features.Interfaces.Managers;

namespace Laba_3_OOP_FishStore.Features.Managers
{
    public class UserManager : IUserManager
    {
        private readonly IMapper _mapper;
        private readonly IUserRepository _userRepository;
        private readonly IUserService _userService;
        private readonly DataContext _dataContext;

        public UserManager(IMapper mapper, IUserRepository userRepository, IUserService userService, DataContext dataContext)
        {
            _mapper = mapper;
            _userRepository = userRepository;
            _userService = userService;
            _dataContext = dataContext;
        }

        public void Create(EditUser editUser)
        {
            var user = new User
            {
                IsnNode = editUser.IsnNode ?? Guid.NewGuid(),
                Name = editUser.Name,
                SurName = editUser.SurName,
                Password = editUser.Password,
            };
            _userRepository.Create(_dataContext, user);

            _dataContext.SaveChanges();
        }

        //public void Update(EditCenter updateCenter)
        //{
        //	var center = _mapper.Map<Center>(updateCenter);

        //	_centerRepository.Update(_dataContext, center);

        //	_dataContext.SaveChanges();
        //}

        //public void Delete(Guid isnNode)
        //{
        //	_centerRepository.Delete(_dataContext, isnNode);

        //	_dataContext.SaveChanges();
        //}

        //public CenterDto GetCenter(Guid isnNode)
        //{
        //	var center = _centerRepository.GetById(_dataContext, isnNode);
        //	return _mapper.Map<CenterDto>(center);
        //}

        //public CenterDto[] GetListCenters(CenterFilterDto centerFilter)
        //{
        //	var centers = _centerService.GetCenterQueryble(_dataContext, centerFilter, true)
        //	   .Select(x => new CenterDto
        //	   {
        //		   IsnNode = x.IsnNode,
        //		   Name = x.Name,
        //		   Code = x.Code
        //	   })
        //	   .ToArray();

        //	return centers;
        //}
    }

}
