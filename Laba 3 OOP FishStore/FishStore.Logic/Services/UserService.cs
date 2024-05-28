using FishStore.Logic.DtoModels.Filters;
using FishStore.Logic.Interfaces.Services;
using FishStore.Storage.Database;
using FishStore.Storage.Models;
using Microsoft.EntityFrameworkCore;

namespace FishStore.Logic.Services
{
    public class UserService : IUserService
    {
        public IQueryable<User> GetCenterQueryble(DataContext dataContext, UserFilterDto filter, bool asNoTracking)
        {
            IQueryable<User> query = dataContext.Users;

            if (asNoTracking)
                query = query.AsNoTracking();

            if (!string.IsNullOrEmpty(filter.Name))
                query = query.Where(x => x.Name == filter.Name);

            return query;
        }
    }
}
