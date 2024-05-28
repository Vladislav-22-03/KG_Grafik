using FishStore.Logic.DtoModels.Filters;
using FishStore.Logic.Interfaces.Services;
using FishStore.Storage.Database;
using FishStore.Storage.Models;
using Microsoft.EntityFrameworkCore;

namespace FishStore.Logic.Services
{
    public class CenterService : ICenterService
    {
        public IQueryable<Center> GetCenterQueryble(DataContext dataContext, CenterFilterDto filter, bool asNoTracking)
        {
            IQueryable<Center> query = dataContext.Centers;

            if (asNoTracking)
                query = query.AsNoTracking();

            if (!string.IsNullOrEmpty(filter.Code))
                query = query.Where(x => x.Code == filter.Code);

            return query;
        }
    }
}
