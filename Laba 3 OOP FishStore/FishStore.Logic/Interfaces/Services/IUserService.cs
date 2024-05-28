using FishStore.Logic.DtoModels.Filters;
using FishStore.Storage.Database;
using FishStore.Storage.Models;

namespace FishStore.Logic.Interfaces.Services
{
    public interface IUserService
    {
        IQueryable<User> GetCenterQueryble(DataContext dataContext, UserFilterDto filter, bool asNoTracking);
    }
}
