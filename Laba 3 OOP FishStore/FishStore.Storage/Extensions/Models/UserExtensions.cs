using FishStore.Storage.Models;

namespace FishStore.Storage.Extensions.Models
{
    public static class UserExtensions
    {
        public static string GetFio(this User user)
        {
            return string.Join(" ", (new string[] {
                user.Name,
                user.SurName,
                user.Password
            }).Where(x => !string.IsNullOrEmpty(x)));
        }
    }
}
